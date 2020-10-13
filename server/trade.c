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
#include"city.h"

void test1(){
	printf("New turn ------------------------------------------\n");
	players_iterate(pplayer) {
    	printf("Player name %s\n", pplayer->name);
		city_list_iterate(pplayer->cities, pcity) {
		  char* city_name = pcity->name;
		  int food = pcity->prod[O_FOOD];
		  int food_surplus = pcity->surplus[O_FOOD];
		  printf("  City name: %s\n    Food: %d\n    Food surplus: %d\n", city_name, food, food_surplus);
		} city_list_iterate_end;
    
    	
    } players_iterate_end;
}

void test_output(){
	printf("New turn ------------------------------------------\n");
	players_iterate(pplayer) {

    	printf("Player name %s\n", pplayer->name);
		city_list_iterate(pplayer->cities, pcity) {
		  char* city_name = pcity->name;
		  int food_prod = pcity->prod[O_FOOD];
		  int food_stock = pcity->food_stock;
		  int food_surplus = pcity->surplus[O_FOOD];

		  int shield_prod = pcity->prod[O_SHIELD];
		  int shield_stock = pcity->shield_stock;
		  int shield_surplus = pcity->surplus[O_SHIELD];

		  printf("  City name: %s\n    Food prod: %d\n    Food stock: %d\n    Food surplus: %d\n", city_name, food_prod, food_stock, food_surplus);
		  printf("    Shield prod: %d\n    Shield stock: %d\n    Shield surplus: %d\n", shield_prod, shield_stock, shield_surplus);
		} city_list_iterate_end;

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



int count_player_cities(const struct player *pplayer){
	int city_count = 0;
	city_list_iterate(pplayer->cities, pcity) {
		  city_count++;
	} city_list_iterate_end;
	return city_count;
}


//take mode defines priority in taking resources
void take_resource(const struct player *pplayer,int resource_type, int resource_amount, int take_mode){
	if(count_player_cities(pplayer) > 0)
	switch(resource_type){
		case 1://food
			switch(take_mode){
				case 1:
					take_food_mode_1(pplayer, resource_amount);
					break;
				default:
					printf("No such take_mode");
					break;
				}
				break;
		case 2://shield
			printf("shields not supported(yet)\n");
			break;
		default:
			printf("No such resource type\n");
			break;
	}
}

void take_food_mode_1(struct player *pplayer ,int food_amount){
	for(int i = 0;i < food_amount; i++){
		struct city *max_food_stock_city;
		int max_food_stock = 0;

		city_list_iterate(pplayer->cities, pcity) {
			if(pcity->food_stock > max_food_stock){
				max_food_stock = pcity->food_stock;
	  			max_food_stock_city = pcity;
	  		}
		} city_list_iterate_end;
		if(max_food_stock_city->food_stock > 0)
			max_food_stock_city->food_stock--;
		else
			printf("food stock empty!\n");
			continue;
	}

}


void distribute_resource(const struct player *pplayer, int resource_type, int resource_amount, int distribute_mode){
	printf("%d\n", count_player_cities(pplayer));
	if(count_player_cities(pplayer) > 0)
		switch(resource_type){
			case 1://food
				switch(distribute_mode){
					case 1:
						distribute_food_mode_1(pplayer, resource_amount);
						break;
					default:
						printf("No such take_mode");
						break;
				}
				break;
			case 2://shield
				printf("shields not supported(yet)\n");
				break;
			default:
				printf("No such resource type\n");
				break;
		}
}

void distribute_food_mode_1(struct player *pplayer, int food_amount){
	for(int i = 0;i < food_amount; i++){
		struct city *min_food_stock_city;
		int min_food_stock = 111110;
		city_list_iterate(pplayer->cities, pcity) {
			int granagy_size = city_granary_size(city_size_get(min_food_stock_city));

			if(pcity->food_stock < min_food_stock){
				min_food_stock = pcity->food_stock;
	  			min_food_stock_city = pcity;
	  		}
		} city_list_iterate_end;
		printf("%d\n", min_food_stock_city->food_stock);
		int granagy_size = city_granary_size(city_size_get(min_food_stock_city));
		if (granagy_size > min_food_stock_city->food_stock)
			min_food_stock_city->food_stock++;
		else
			printf("food stock full!\n");
			continue;
	}
}

//food production in city: pcity->prod[O_FOOD]
//food supplies in city: pcity->food_stock
//food surplus in city: pcity->surplus[O_FOOD]

//shield production in city: pcity->prod[O_SHIELD]
//shield supply in city: pcity->shield_stock
//shield surplus in city: pcity->surplus[O_SHIELD]

int calculate_buy_price(int recourse_type){//1=food; 2=shield
	static int k = 1;
	int price;
	int total_prod = 0, total_cons = 0, total_gold_earned = 0;
	//gather ststs
	if(recourse_type == 1){
		players_iterate(pplayer) {
			city_list_iterate(pplayer->cities, pcity) {

			  int food_prod = pcity->prod[O_FOOD];
			  int food_stock = pcity->food_stock;
			  int food_surplus = pcity->surplus[O_FOOD];

			  int shield_prod = pcity->prod[O_SHIELD];
			  int shield_stock = pcity->shield_stock;
			  int shield_surplus = pcity->surplus[O_SHIELD];

			} city_list_iterate_end;
		} players_iterate_end;
	}else if(recourse_type == 2){

		//aaaaaaaaaaaaaaaaaaa
	}
    //calculate price with complex formula
    price = k;
    k++;
	return price;
}

int calculate_sell_price(int recourse_type){
	static int k = 1;
	int price;
	int total_prod = 0, total_cons = 0, total_gold_earned = 0;
	//gather ststs
	/*if(recourse_type == 1){
		players_iterate(pplayer) {
			city_list_iterate(pplayer->cities, pcity) {

			  int food_prod = pcity->prod[O_FOOD];
			  int food_stock = pcity->food_stock;
			  int food_surplus = pcity->surplus[O_FOOD];

			  int shield_prod = pcity->prod[O_SHIELD];
			  int shield_stock = pcity->shield_stock;
			  int shield_surplus = pcity->surplus[O_SHIELD];

			} city_list_iterate_end;
		} players_iterate_end;
	}else if(recourse_type == 2){

		//aaaaaaaaaaaaaaaaaaa
	}*/
    //calculate price with complex formula
    price = k;
    k++;
	return price;
}

void update_price(struct player *pplayer){
	
	int buy_price = calculate_buy_price(1); 
		
	pplayer->price_food_buy = buy_price;
		printf("----\n");

	int sell_price = calculate_sell_price(1); 
		printf("----\n");
	pplayer->price_food_sell = sell_price;
		printf("!!!\n");

}

