/**
 * codebot.js
 * Defines codebot object and controls render
 * 12/13/12 Mojiferous
 */
var player_bot;

(function ($, Drupal, window, document) {

  Drupal.behaviors.codeBot = {
    attach: function (context) {
      if(Drupal.settings.cache_val) {
//        run_bots(Drupal.settings.cache_val);
      }
    }
  };

})(jQuery, Drupal, this, this.document);

/**
 * run a codebot line for all bots, passed a cache name
 * @param cache_name int
 */
function run_bots(cache_name) {
  jQuery.get(Drupal.settings.basePath+'code/'+cache_name+'/step', function(data, status) {
    jQuery('#code_result').append(data+'<br/>');
  });
//  setTimeout('run_bots('+cache_name+')', 1000);
}