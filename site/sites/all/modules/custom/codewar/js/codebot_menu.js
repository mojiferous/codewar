/**
 * JS handler for the menu and general page functions
 * 12/23/13 Mojiferous
 */

(function ($, Drupal, window, document) {

  $(window).load(function() {
    $('#bot-home-button').button();
    $('#bot-edit-button').button({
      icons: {
        primary: 'ui-icon-document'
      }
    });
    $('#bot-icon-button').button({
      icons: {
        primary: 'ui-icon-image'
      }
    });
    $('#bot-attr-button').button({
      icons: {
        primary: 'ui-icon-wrench'
      }
    });
  });

})(jQuery, Drupal, this, this.document);