/**
 * codebot_edit.js
 * Defines codebot editor and controls render through Ace
 * 8/23/12 Mojiferous
 */
var player_bot;

(function ($, Drupal, window, document) {

  Drupal.behaviors.codeBot = {
    attach: function (context) {
      $(window).load(function() {
        var editor = ace.edit("code-body");
        editor.setTheme("ace/theme/kr_theme");
        editor.getSession().setMode("ace/mode/codewar");
      });

    }
  };

})(jQuery, Drupal, this, this.document);
