#ifndef FC__TRADE_H
#define FC__TRADE_H

void test1();
bool players_can_trade(const struct player *pplayer, const struct player *pplayer2);
void test_output();
void distribute_resource(const struct player *pplayer, int resource_type, int resource_amount, int distribute_mode);
void take_resource(const struct player *pplayer, int resource_type, int resource_amount, int take_mode);
int count_player_cities(const struct player *pplayer);
void update_price(pplayer);

#endif 
