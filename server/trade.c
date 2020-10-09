/* utility */
#include "astring.h"
#include "bitvector.h"
#include "bugs.h"
#include "capability.h"
#include "fc_cmdline.h"
#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "mem.h"
#include "netintf.h"
#include "rand.h"
#include "registry.h"
#include "support.h"
#include "timing.h"
/*common*/
#include"player.h"

void test1(){
	printf("Test succesfull!\n");
	int food = 0, shields = 0;
	players_iterate(pplayer) {
    
    city_list_iterate(pplayer->cities, pcity) {
      shields += pcity->prod[O_SHIELD];
      food += pcity->prod[O_FOOD];
    } city_list_iterate_end;
    
    	printf("food: %d\nshields: %d\n=============\n", food, shields);
    	food = 0;
    	shields = 0;
    	
    } players_iterate_end;
}

bool is_dipl_contact_established(const struct player *pplayer, const struct player *pplayer2){
	enum diplstate_type ds;
	ds = player_diplstate_get(pplayer, pplayer2)->type;
	if(ds == DS_NO_CONTACT)
		return FALSE;
	return TRUE;
}

bool is_allie_at_war_with_player(const struct player *pplayer, const struct player *pplayer2){
	players_iterate(pplayer_iter){
		if(pplayers_allied(pplayer, pplayer_iter) == TRUE)
			if(pplayers_at_war(pplayer_iter, pplayer2) == TRUE)
				return TRUE;
	}players_iterate_end;
	return FALSE;
}

bool players_can_trade(const struct player *pplayer, const struct player *pplayer2){
	if(is_dipl_contact_established(pplayer, pplayer2) == TRUE)
		if( pplayers_at_war(pplayer, pplayer2) == FALSE && is_allie_at_war_with_player(pplayer, pplayer2) == FALSE)
			return TRUE;
	return FALSE;
}