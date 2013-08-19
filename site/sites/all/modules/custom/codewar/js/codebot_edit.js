/**
 * codebot.js
 * Defines codebot object and controls render
 * 12/13/12 Mojiferous
 */
var player_bot;

(function ($, Drupal, window, document) {

  Drupal.behaviors.codeBot = {
    attach: function (context) {
      var editor = ace.edit("code-body");
      editor.setTheme("ace/theme/kr_theme");
      editor.getSession().setMode("ace/mode/codewar");
    }
  };

})(jQuery, Drupal, this, this.document);
