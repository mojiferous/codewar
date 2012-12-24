/**
 * codebot.js
 * Defines codebot object and controls render
 * 12/13/12 Mojiferous
 */

//to get keys of passed object: Object.keys(Drupal.settings.bot_code) -- returns array

(function ($, Drupal, window, document, undefined) {

  function Codebot(this_obj) {
    this.posx = 0;
    this.posy = 0;
    this.spdx = 0;
    this.spdy = 0;
    this.obj = this_obj;
  }

  Codebot.prototype.set_x = function(x_loc) {
    this.posx = x_loc;
    $(this.obj).css({left: this.posx});
  };

  Codebot.prototype.set_y = function(y_loc) {
    this.posy = y_loc;
    $(this.obj).css({top : this.posy});
  };

  Codebot.prototype.set_position = function(x_loc, y_loc) {
    this.set_x(x_loc);
    this.set_y(y_loc);
  }

  Codebot.prototype.add_code = function(code) {
    this.raw_code = code;
  }

})(jQuery, Drupal, this, this.document);

