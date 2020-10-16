#ifndef FC__TRADE_H
#define FC__TRADE_H


struct player_resource_chars{
	int resource_type;
	int surplus_cities_count;
	int shoretage_cities_count;
	int neutral_city_count;
	int total_resource_delta;
}player_resource_chars;

#define SPECLIST_TAG player_resource_chars
#define SPECLIST_TYPE struct player_resource_chars
#include "speclist.h"

void test1();
bool players_can_trade(const struct player *pplayer, const struct player *pplayer2);
void test_output();
void distribute_resource(const struct player *pplayer, int resource_type, int resource_amount, int distribute_mode);
void take_resource(const struct player *pplayer, int resource_type, int resource_amount, int take_mode);
int count_player_cities(const struct player *pplayer);
void update_price(struct player *pplayer);
int count_player_resource_chars(struct player *pplayer, int resource_type);
void make_player_eco_report(struct player* pplayer, struct player_resource_chars *pl_rc_ch, int resource_type);
void print_player_resource_chars(struct player_resource_chars *pl_rc_ch);

#endif 
