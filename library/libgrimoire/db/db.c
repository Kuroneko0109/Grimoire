#include <stdlib.h>
#include <string.h>

#include <libgrimoire/db/db.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_db priv_db_t;
struct priv_db {
	db_t public;

	lock_t * query_lock;
	list_t * query_queue;

	list_t * (*real_query)(db_t *, const char *, void * (*)(MYSQL_ROW));

	MYSQL * conn;
	char db_ip[32];
	unsigned int db_port;
	char db_base[64];
	char db_id[64];
	char db_pwd[128];
};

list_t * db_real_query(db_t * this, const char * query, void * (*db_result_wrapper)(MYSQL_ROW))
{
	priv_db_t * priv = (priv_db_t *)this;
	list_t * ret = NULL;
	int res;
	MYSQL_RES * result;
	int rows_cnt;
	MYSQL_ROW row;
	void * data;

	priv->query_lock->lock(priv->query_lock);
	res = mysql_query(priv->conn, query);
	priv->query_lock->unlock(priv->query_lock);

	if(0 == res)
	{
		if(db_result_wrapper)
		{
			result = mysql_store_result(priv->conn);
			ret = create_list(LOCK_SPINLOCK, NULL, NULL);
			ret->lock(ret);
			while((row = mysql_fetch_row(result)))
			{
				data = db_result_wrapper(row);
				ret->enqueue_data(ret, data);
			}
			ret->unlock(ret);
			mysql_free_result(res);
		}
	}
	else
	{
		printf("Query failure(%s)\n", query);
	}

	return ret;
}

list_t * db_squery(db_t * this, const char * query, void * (*db_result_wrapper)(MYSQL_ROW))
{
	priv_db_t * priv = (priv_db_t *)this;

	priv->real_query(this, query, db_result_wrapper);

	return 0;
}

int db_aquery(db_t * this, const char * query)
{
	priv_db_t * priv = (priv_db_t *)this;
	int query_size = strlen(query) + 1;
	char * data = malloc(query_size);
	strcpy(data, query);

	priv->query_queue->lock(priv->query_queue);
	priv->query_queue->enqueue_data(priv->query_queue, data);
	priv->query_queue->unlock(priv->query_queue);

	return 0;
}

void * db_thread(void * param)
{
	db_t * this = (db_t *)param;
	priv_db_t * priv = (priv_db_t *)this;
	char * query;

	if(NULL == priv->conn)
		return NULL;

	priv->query_queue->lock(priv->query_queue);
	query = priv->query_queue->dequeue_data(priv->query_queue);
	priv->query_queue->unlock(priv->query_queue);
	priv->real_query(this, query, NULL);
	free(query);

	return NULL;
}

void db_set_conn_info(db_t * this, const char * ip, unsigned int port, const char * base, const char * id, const char * pwd)
{
	priv_db_t * priv = (priv_db_t *)this;

	memset(priv->db_ip, 0, sizeof(priv->db_ip));
	priv->db_port = 0;
	memset(priv->db_base, 0, sizeof(priv->db_base));
	memset(priv->db_id, 0, sizeof(priv->db_id));
	memset(priv->db_pwd, 0, sizeof(priv->db_pwd));

	strcpy(priv->db_ip, ip);
	priv->db_port = port;
	strcpy(priv->db_base, base);
	strcpy(priv->db_id, id);
	strcpy(priv->db_pwd, pwd);
}

int db_connect(db_t * this)
{
	priv_db_t * priv = (priv_db_t *)this;
	priv->conn = mysql_init(NULL);
	if(NULL == priv->conn)
		return -1;

	if(NULL == mysql_real_connect(priv->conn,
				priv->db_ip,
				priv->db_id,
				priv->db_pwd,
				strlen(priv->db_base)?priv->db_base:NULL,
				priv->db_port,
				NULL,
				0))
		return -1;

	return 0;
}

void db_disconnect(db_t * this)
{
	priv_db_t * priv = (priv_db_t *)this;
	mysql_close(priv->conn);
	priv->conn = NULL;
}

int db_get_error(db_t * this, char * dst)
{
	priv_db_t * priv = (priv_db_t *)this;
	if(NULL != dst)
		strcpy(dst, mysql_error(priv->conn));
	return strlen(mysql_error(priv->conn));
}

void db_destroy(db_t * this)
{
	priv_db_t * priv = (priv_db_t *)this;
	if(priv->conn)
		this->disconnect(this);
	free(this);
}

db_t * create_db(void)
{
	db_t * public;
	priv_db_t * private;

	private = malloc(sizeof(priv_db_t));
	public = &private->public;

	private->query_lock = create_lock(LOCK_SPINLOCK);
	private->query_queue = create_list(LOCK_MUTEX, NULL, NULL);
	private->conn = mysql_init(NULL);
	private->real_query = db_real_query;

	public->aquery = db_aquery;
	public->squery = db_squery;
	public->get_error = db_get_error;
	public->connect = db_connect;
	public->disconnect = db_disconnect;
	public->set_conn_info = db_set_conn_info;
	public->destroy = db_destroy;

	return public;
}
