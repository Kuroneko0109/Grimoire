#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/network/analyzer.h>
#include <libgrimoire/network/pkt_profile.h>

#include <stdlib.h>

typedef struct priv_analyzer priv_analyzer_t;

struct priv_analyzer {
	analyzer_t public;

	hashlist_t * hashlist;
};

void analyzer_input(analyzer_t * this, pkt_profile_t * profile)
{
	priv_analyzer_t * priv = (priv_analyzer_t *)this;
}

void analyzer_hasher(void * data)
{
	pkt_profile_t * profile = (pkt_profile_t *)data;
}

analyzer_t * create_analyzer(void)
{
	priv_analyzer_t * private;
	analyzer_t * public;

	private = malloc(sizeof(priv_analyzer_t));
	public = &private->public;

	private->hashlist = create_hashlist(analyzer_hasher, 256, NULL, NULL, NULL);

	return public;
}
