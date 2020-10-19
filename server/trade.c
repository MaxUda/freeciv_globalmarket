#include<math.h>

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
#include"trade.h"

int count_total_usage();
int count_total_production();
int count_total_surp_player(struct player* pplayer);
int count_total_usage_player(struct player* pplayer);
int count_total_production_player(struct player* pplayer);
int player_food_count(struct player* pplayer);
int calculate_price(int stock, int prev_price, float delta_k);




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

/*********************************************************
						MANAGE RESOURCES
**********************************************************/

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
		else{
			printf("food stock empty!\n");
			continue;
		}
	}

}


void distribute_resource(const struct player *pplayer, int resource_type, int resource_amount, int distribute_mode){
	//printf("%d\n", count_player_cities(pplayer));
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
		int granagy_size = city_granary_size(city_size_get(min_food_stock_city));
		if (granagy_size > min_food_stock_city->food_stock)
			min_food_stock_city->food_stock++;
		else{
			printf("food stock full!\n");
			continue;
		}
	}
}

//food production in city: pcity->prod[O_FOOD]
//food supplies in city: pcity->food_stock
//food surplus in city: pcity->surplus[O_FOOD]

//shield production in city: pcity->prod[O_SHIELD]
//shield supply in city: pcity->shield_stock
//shield surplus in city: pcity->surplus[O_SHIELD]

void commit_bargains(int resource_type)
{
	players_iterate(pplayer){
		switch(resource_type){
			case 1://food
				if(pplayer->delta_food == 0)
					continue;
				//printf("%d\n", pplayer->delta_food);
				int delta_food = pplayer->delta_food;
				int sell_price = pplayer->price_food_sell;
				int buy_price = pplayer->price_food_buy;
				if(pplayer->delta_food > 0){
					if(pplayer->economic.gold > delta_food*sell_price*2)
						pplayer->economic.gold -= (delta_food*sell_price);
					else
						printf("NO gold!\n");
					distribute_resource(pplayer, 1, delta_food, 1);
				}else if(pplayer->delta_food < 0){
					pplayer->economic.gold += -(delta_food*buy_price);
					take_resource(pplayer, 1, -delta_food, 1);
				}
				pplayer->delta_food = 0;
				printf("commit_bargans\n");
				break;
			default:
				break;
		}
	}players_iterate_end;
}

/*********************************************************
						MANAGE PRICE
**********************************************************/

/*int calculate_buy_price(struct player *pplayer, int recourse_type){//1=food; 2=shield
	static int k = 1;
	int price;
	int total_prod = 0, total_cons = 0, total_gold_earned = 0;
	
    price = k;
    k++;
	return pplayer->price_food_buy + 10;
}

int calculate_sell_price(struct player *pplayer, int recourse_type){
	static int k = 1;
	int price;
	int total_prod = 0, total_cons = 0, total_gold_earned = 0;
	
    price = k;
    k++;
	return pplayer->price_food_sell + 10;
}*/

int calculate_price(int stock, int prev_price, float delta_k)
{
	//int price =  ceil(abs((stock + count_total_production())/count_total_usage()));
	//int price =  ceil(abs((stock + count_total_production())/count_total_usage()));
	/*float c_t_p = count_total_production();
	float c_t_u = count_total_usage();
	float stock1 = stock;
	double delta_price = count_total_usage()/(count_total_production() - 15 + stock1);
	double PP = prev_price;
	int price = 7*delta_price;
	printf("Price: %f\n", delta_price);*/
	//printf("Total prod %d\nTotal use %d\nPreice %d\n", count_total_production(), count_total_usage(), price);


	float p_price = prev_price;
	
	int price = ceil(p_price*(1 + 100*delta_k));
	printf("P_Price %f\n", delta_k);
	return price;
}

void update_price(struct player *pplayer, float s, int stock)
{
	//static float k_prev = 0;
	//float k = (float)count_total_usage()/(float)count_total_production();
	//printf("Stock: %d\n", stock);
	printf("Stock: %d\n", stock);
	//printf("k_prev: %f k: %f\n", k_prev, k);
	
	int price = calculate_price(stock, pplayer->price_food_buy, s);
	if(price < 1)
		price = 1;
	pplayer->price_food_buy = price;
	pplayer->price_food_sell = price;
	//stock++;
	//k_prev = (float)count_total_usage()/(float)count_total_production();
}



/*********************************************************
						AI
**********************************************************/


void make_ai_trade(int stock)
{
	players_iterate(pplayer) {
		if (!is_ai(pplayer) || is_barbarian(pplayer)) {
      		continue;
    	}
    	ai_player_trade_1(pplayer, stock);
    } players_iterate_end;
}

void ai_player_trade_1(struct player *pplayer, int stock)
{	
	float pl_use = (float)count_total_usage_player(pplayer);
	float pl_prod = (float)count_total_production_player(pplayer);
	float t_use = count_total_usage();
	float t_prod = count_total_production();
	float k = t_use/t_prod;
	float pl_k = pl_use/pl_prod;
	printf("%f - %f - %f - %f - %f\n", pl_use, pl_prod, pl_k, ceil(pl_prod*(pl_k - k)), k);
	if(pl_k > k){
		/*if(/*fabs(t_use*pl_prod - pl_use*t_prod) < fabs(t_prod*pl - pl_prod) && stock < -100){
		printf("a %f b %f\n", fabs(t_use - pl_use), fabs(t_prod - pl_prod));
			pplayer->delta_food = ceil(pl_prod*(pl_k - k));
			printf("+\n");
		}*/

		//pplayer->delta_food = ceil(pl_prod*(pl_k - k));
		//if(pplayer->delta_food*pplayer->price_food_buy > pplayer->economic.gold)
			//pplayer->delta_food = 0;
		//printf("%f\n", ceil(abs(1-pl_k)*pl_prod)*3);
	}else if (pl_k < k){
		/*if(/*fabs(t_use - pl_use) < fabs(t_prod - pl_prod) stock > 100){
			pplayer->delta_food = ceil(pl_prod*(pl_k - k));
			printf("-\n");
		}*/
		//pplayer->delta_food = -ceil(pl_use*(k - pl_k));
		//if(pplayer->delta_food > player_food_count(pplayer))
		//	pplayer->delta_food = 0;
		//printf("%d\n", pplayer->delta_food);
	}
}

void ai_player_trade_2(struct player *pplayer)
{
	if(count_total_surp_player(pplayer) > 10){
		pplayer->delta_food = -count_total_surp_player(pplayer)/2;
	}else if(count_total_surp_player(pplayer) <= 1){
		pplayer->delta_food = count_total_surp_player(pplayer);
	}
}

void ai_player_trade_3(struct player *pplayer)
{
	if(count_total_surp_player(pplayer) > 10){
		pplayer->delta_food = -count_total_surp_player(pplayer)/2;
	}else if(count_total_surp_player(pplayer) <= 1){
		pplayer->delta_food = count_total_surp_player(pplayer);
	}
}


/*********************************************************
						ECO MISC
**********************************************************/

int count_player_resource_chars(struct player *pplayer, int resource_type)
{
	int total_surplus = 0;
	city_list_iterate(pplayer->cities, pcity){
		switch(resource_type){
			case 1:
				total_surplus += pcity->surplus[O_FOOD];
				break;
			default:
				break;
		}
	}city_list_iterate_end;
	return total_surplus;
} 

/*struct player_resource_chars{
	int resource_type;
	int surplus_cities_count;
	int shoretage_cities_count;
	int neutral_city_count;
	int total_resource_delta;
};*/

void make_player_eco_report(struct player* pplayer, struct player_resource_chars *pl_rc_ch, int resource_type)
{

	    printf("/\n");
	pl_rc_ch->surplus_cities_count = 0;
	pl_rc_ch->total_resource_delta = 0;
	pl_rc_ch->shoretage_cities_count = 0;
	pl_rc_ch->neutral_city_count = 0;
		printf("/\n");
	city_list_iterate(pplayer->cities, pcity){
		if(pcity->surplus[O_FOOD] > 0){
			pl_rc_ch->surplus_cities_count++;
			pl_rc_ch->total_resource_delta += pcity->surplus[O_FOOD];
		}else if(pcity->surplus[O_FOOD] < 0){
			pl_rc_ch->shoretage_cities_count++;
			pl_rc_ch->total_resource_delta += pcity->surplus[O_FOOD];
		}else{
			pl_rc_ch->neutral_city_count++;
		}
	}city_list_iterate_end;

	pl_rc_ch->resource_type = resource_type;
}

void print_player_resource_chars(struct player_resource_chars *pl_rc_ch)
{
	printf("Total food delta: %d\n", pl_rc_ch->total_resource_delta);
	printf("Surplus cities count: %d\n", pl_rc_ch->surplus_cities_count);
	printf("shoretage_cities_count: %d\n", pl_rc_ch->shoretage_cities_count);
	printf("Neutral city count: %d\n", pl_rc_ch->neutral_city_count);
}

int count_total_production()
{
	int total_prod = 0;
	players_iterate(pplayer){
		city_list_iterate(pplayer->cities, pcity){
			total_prod += pcity->prod[O_FOOD];
		}city_list_iterate_end;
	}players_iterate_end;
	return total_prod;
}

int count_total_usage()
{
	int total_use = 0;
	players_iterate(pplayer){
		city_list_iterate(pplayer->cities, pcity){
			total_use += pcity->usage[O_FOOD];
		}city_list_iterate_end;
	}players_iterate_end;
	return total_use;
}

int count_total_usage_player(struct player* pplayer)
{
	int total_use = 0;
	
	city_list_iterate(pplayer->cities, pcity){
		total_use += pcity->usage[O_FOOD];
	}city_list_iterate_end;
	
	return total_use;
}
int count_total_production_player(struct player* pplayer)
{
	int total_prod = 0;
	
	city_list_iterate(pplayer->cities, pcity){
		total_prod += pcity->prod[O_FOOD];
	}city_list_iterate_end;
	
	return total_prod;
}

int count_total_surp_player(struct player* pplayer)
{
	int total_surp = 0;
	
	city_list_iterate(pplayer->cities, pcity){
		total_surp += pcity->surplus[O_FOOD];
	}city_list_iterate_end;
	
	return total_surp;
}

int player_food_count(struct player* pplayer)
{
	int total_surp = 0;
	
	city_list_iterate(pplayer->cities, pcity){
		total_surp += pcity->food_stock;
	}city_list_iterate_end;
	
	return total_surp;
}