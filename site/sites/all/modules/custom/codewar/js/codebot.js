/**
 * codebot.js
 * Defines codebot object and controls render
 * 12/13/12 Mojiferous
 */
var player_bot;

(function ($, Drupal, window, document) {

  Drupal.behaviors.codeBot = {
    attach: function (context) {
      if(Drupal.settings.bot_1) {
        run_bot(Drupal.settings.bot_1);
      }
    }
  };

  /**
   * run a codebot line for bot # number
   * @param number int
   */


})(jQuery, Drupal, this, this.document);

function run_bot(number) {
  jQuery.get(Drupal.settings.basePath+'/step/'+number, function(data, status) {
    jQuery('#code_result').append(data+'<br/>');
  });
//  setTimeout('run_bot('+number+')', 1000);
}