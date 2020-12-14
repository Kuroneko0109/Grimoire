#include <stdlib.h>
#include <string.h>

#include <libgrimoire/db/db.h>
#include <libgrimoire/datastructure/list.h>

#include <mysql/mysql.h>

typedef struct priv_db priv_db_t;
struct priv_db {
	db_t public;

	list_t * query_queue;

	char db_ip[32];
	unsigned int db_port;
	char db_base[64];
	char db_id[64];
	char db_pwd[128];
	MYSQL * conn;
};

int db_query(db_t * this, const char * query)
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

int db_execute(db_t * this)
{
	priv_db_t * priv = (priv_db_t *)this;
	char * data;
	int counter = 0;
	int query_ret = 0;

	if(NULL == priv->conn)
		return -1;

	priv->query_queue->lock(priv->query_queue);
	while((data = priv->query_queue->dequeue_data(priv->query_queue)))
	{
		priv->query_queue->unlock(priv->query_queue);
		if(NULL == data)
			goto end;
		do {
			query_ret = mysql_query(priv->conn, data);
			if(0 != query_ret)
			{
			}
		} while(0);
		counter++;
	}
end :
	return counter;
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

	if(NULL == mysql_real_connect(priv->conn, priv->db_ip, priv->db_id, priv->db_pwd, priv->db_base, priv->db_port, NULL, 0))
		return -1;

	return 0;
}

void db_disconnect(db_t * this)
{
	priv_db_t * priv = (priv_db_t *)this;
	mysql_close(priv->conn);
	priv->conn = NULL;
}

int get_error(db_t * this, char * dst)
{
	priv_db_t * priv = (priv_db_t *)this;
	strcpy(dst, mysql_error(priv->conn));
	return strlen(dst);
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

	private->query_queue = create_list(LOCK_MUTEX, NULL, NULL);
	private->conn = mysql_init(NULL);

	public->query = db_query;
	//public->fquery = db_fquery;
	public->connect = db_connect;
	public->disconnect = db_disconnect;
	public->set_conn_info = db_set_conn_info;
	public->destroy = db_destroy;

	return public;
}
