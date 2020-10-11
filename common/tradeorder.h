#ifndef FC__TRADEORDER_H
#define FC__TRADEORDER_H

struct order{
	int id;
	int player_id;
	int order_type;
	int recourse_type;
	int recourse_amount;
	int timer;//how many turns will the order last

	int *partner_ids;//they have singed under the order
	int *who_can_trade;//they hfave access to the order
}

#endif 