<?php
/**
 * code_map.inc
 * handles the map and related functions -
 * 8/13/13 Mojiferous
 */

//define our global values
define('CW_MAP_WIDTH', 30); //<int the width of the map
define('CW_MAP_HEIGHT', 30); //<int the height of the map
define('CW_MAP_DEPTH', 2); //<int the depth of the map

require_once('code_bot.inc');

class code_map {

  public $map = array(); //<array of map values, [x][y][level] where level 0==terrain 1==bots 2==bombs/shots/etc.
  public $bots = array(); //<array of code_bot objects

  public function __construct($cached_data = null) {
    if($cached_data) {
      if(isset($cached_data->map) && isset($cached_data->bots)) {
        $this->map = $cached_data->map;
        $this->bots = $cached_data->bots;
      }

    } else {
      $this->new_map();
    }

  }

  /**
   * instantiate a new map object
   */
  public function new_map() {
    //set default values for the map array
    for($x=0; $x<=CW_MAP_WIDTH; $x++) {
      for($y=0; $y<=CW_MAP_HEIGHT; $y++) {

        for($z=0; $z<=CW_MAP_DEPTH; $z++) {
          $this->map[$x][$y][$z] = 0;
        }

      }
    }

  }

  /**
   * add a previously loaded code_bot to the array, and assign it a location
   * @param code_bot $bot
   * @param int $bot_hp
   * @param $bot_armor
   * @param $bot_speed
   * @param $bot_ranged
   * @param $bot_mid
   * @param $bot_mine
   * @param $bot_bomb
   */
  public function add_new_bot(code_bot $bot, $bot_hp, $bot_armor, $bot_speed, $bot_ranged, $bot_mid, $bot_mine, $bot_bomb) {
    if($bot && isset($bot->code_array)) {
      //make sure the bot is set and has a code array
      $this->bots[] = $bot;

      $placed = false;
      $this_x = 0;
      $this_y = 0;

      do {
        //get a random value and cast them as ints
        $this_x = (int)round(rand(0,CW_MAP_WIDTH));
        $this_y = (int)round(rand(0,CW_MAP_HEIGHT));

        if(isset($this->map[$this_x][$this_y][1]) && $this->map[$this_x][$this_y][1] == 0) {
          $placed = true;
        }

      } while(!$placed);

      if(isset($this->map[$this_x][$this_y][1])) {
        //place the bot on the map -- placed as the index+1 of the bot (since index 0 will be count 1)
        $this->map[$this_x][$this_y][1] = count($this->bots);

        $bot->set_location($this_x, $this_y);
        $bot->set_hp($bot_hp);
        $bot->set_armor($bot_armor);
        $bot->set_speed($bot_speed);
        $bot->set_weapons(array($bot_ranged, $bot_mid, $bot_mine, $bot_bomb));

        $this->bots[count($this->bots)-1] = $bot;

      }

    }
  }

  /**
   * handles an individual turn for a bot
   */
  public function handle_bot_turns() {
    for($n=0; $n<count($this->bots); $n++) {
      $this_bot = new code_bot('');
      $this_bot->rebuild_from_cache($this->bots[$n]);

      $result_code = $this_bot->run_code();

      $this->bots[$n] = $this_bot;

      dpm($result_code);
    }

  }
  
}
