/**
 * JS handler for the Codebot home screen
 * 1/2/14 Mojiferous
 */

(function ($, Drupal, window, document) {

  $(window).load(function() {
    $('.bot-selector').chosen({
      no_results_text: 'No matching Codebots found!',
      allow_single_deselect: true
    });
  });

})(jQuery, Drupal, this, this.document);