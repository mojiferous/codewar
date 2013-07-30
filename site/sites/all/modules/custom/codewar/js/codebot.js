/**
 * codebot.js
 * Defines codebot object and controls render
 * 12/13/12 Mojiferous
 */
var player_bot;

(function ($, Drupal, window, document, undefined) {

  /**
   * instantiate a new codebot object
   * @param {object} this_obj the
   * @constructor
   */
  function Codebot(this_obj) {
    this.posx = 0;
    this.posy = 0;
    this.spdx = 0;
    this.spdy = 0;
    this.obj = this_obj;
    this.funcs = new Array();
    this.vars = {};
    this.cur_func = 'main';
    this.cur_loc = [0]

    this.parse_funcs();
    this.parse_vars();
  }

  /**
   * set the x location of the bot
   * @param x_loc
   */
  Codebot.prototype.set_x = function(x_loc) {
    this.posx = x_loc;
    $(this.obj).css({left: this.posx});
  };

  /**
   * set the y location of the bot
   * @param y_loc
   */
  Codebot.prototype.set_y = function(y_loc) {
    this.posy = y_loc;
    $(this.obj).css({top : this.posy});
  };

  /**
   * set the location (x and y) of the bot
   * @param x_loc
   * @param y_loc
   */
  Codebot.prototype.set_position = function(x_loc, y_loc) {
    this.set_x(x_loc);
    this.set_y(y_loc);
  };

  /**
   * parse the function names from the raw code
   */
  Codebot.prototype.parse_funcs = function() {
    this.funcs = Object.keys(this.obj);

    //remove the vars array from the functions array
    var var_loc = -1;
    for(n=0; n<this.funcs.length; n++) {
      if(this.funcs[n] == 'vars') {
        var_loc = n;
      }
    }

    if(var_loc > -1) {
      this.funcs.splice(var_loc,1);
    }
  };

  /**
   * parse the variable names into an object from the raw code
   */
  Codebot.prototype.parse_vars = function() {
    if(this.obj.vars != null) {
      //make sure there is something inside the variables array
      var var_names = Object.keys(this.obj.vars);

      for(n=0; n<var_names.length; n++) {
        this.vars[var_names[n]] = 0;
      }
    }
  };

  Codebot.prototype.run_line = function() {
    if(is_array(this.obj[this.cur_func])) {
      //make sure our function actually exists
      if(this.obj[this.cur_func][this.cur_loc[this.cur_loc.length-1]] != null) {
        //make sure our current line exists... this should always be true

        console.log(this.obj[this.cur_func][this.cur_loc[this.cur_loc.length-1]]);

        this.move_line();
      }
    }


  };

  Codebot.prototype.move_line = function() {
    if(this.obj[this.cur_func][this.cur_loc[this.cur_loc.length-1]+1] != null) {
      //the next line is not null, go to it
      this.cur_loc[this.cur_loc.length-1]++;

    } else {
      //we're at the end of the array
      this.cur_loc[0] = 0;

    }
  };

  $(document).ready(function() {
    player_bot = new Codebot(Drupal.settings.bot_code);

    for(n=0; n<15; n++) {
      player_bot.run_line();
    }
  });

})(jQuery, Drupal, this, this.document);

/**
 * returns a parsed object type
 * @param obj
 * @return {String}
 */
function return_object_type(obj) {
  var this_type = '';
  this_type = Object.prototype.toString.call(obj);
  this_type = this_type.replace('[object ', '').replace(']', '').toLowerCase();

  return this_type;
}

/**
 * checks if an object is a string
 * @param obj
 * @return {Boolean}
 */
function is_string(obj) {
  if(return_object_type(obj) == 'string') {
    return true;
  }

  return false;
}

/**
 * checks if an object is an object
 * @param obj
 * @return {Boolean}
 */
function is_object(obj) {
  if(return_object_type(obj) == 'object') {
    return true;
  }

  return false;
}

/**
 * checks if an object is an array
 * @param obj
 * @return {Boolean}
 */
function is_array(obj) {
  if(return_object_type(obj) == 'array') {
    return true;
  }

  return false;
}