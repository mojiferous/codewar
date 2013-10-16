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

        $('#save-code').button().click(function() {
          //the save-code button has been clicked, ajax post the contents of the editor

          var request = $.ajax({
            type: "POST",
            url: "/code/"+Drupal.settings.codewar.bot_id+"/save",
            data: {code_data: editor.getValue()},
            success: function(data, text_status) {
              switch (data) {
                //handle errors, success
                case '1':
                  alert('Error: Unable to save: No data passed to server');
                  break;
                case '2':
                  alert('Internal Error: Unable to save bot');
                  break;
                case '3':
                  alert('Error: Unable to save: Data corrupted');
                  break;
                default:
                  location.reload();
                  break;
              }
            }
          });
        });

      });

    }
  };

})(jQuery, Drupal, this, this.document);
