/**
 * codebot_icons.js
 * Defines codebot icon editor and controls render
 * 8/23/12 Mojiferous
 */
var chooser_canvas;
var icon_canvas;
var tot_icons;

(function ($, Drupal, window, document) {

  Drupal.behaviors.codeBot = {
    attach: function (context) {

      $(window).load(function() {
        $('#chose-color').spectrum(
          {
            showPalette: true,
            showSelectionPalette: true,
            palette: [ ],
            localStorageKey: "spectrum.codewar",
            showInitial: true
          }
        );

        //get our canvas objects
        chooser_canvas = document.getElementById('chooser-canvas').getContext('2d');
        icon_canvas = document.getElementById('icon-canvas').getContext('2d');

        var icon_num = 0;

        //add each icon to the icon chooser
        $('#icon-hidey-hole').children('img').each(function() {
          var this_img_id = $(this).attr('id');
          var this_img = document.getElementById(this_img_id);

          //draw the icon
          draw_icon(this_img, icon_num, false);

          icon_num++;
        });

        draw_icon(null, icon_num, true);

        tot_icons = icon_num-1;

        icon_canvas.fillStyle = '#999999';
        icon_canvas.fillRect(0, 0, 354, 354);
      });

    }
  };

  /**
   * draws an icon at icon_num location, uses the Drupal.settings object to determine the currently selected icon
   * @param this_img
   * @param icon_num
   * @param is_new
   */
  function draw_icon(this_img, icon_num, is_new) {
    var selected = (Drupal.settings.codewar.selected_icon == icon_num);
    chooser_canvas.font = '18px Arial';
    chooser_canvas.fillStyle = '#CCCCCC';

    if(selected) {
      //this is our selected icon, change the tab color
      chooser_canvas.fillStyle = '#999999';
    }

    chooser_canvas.fillRect(0, (40*icon_num), 150, 36);

    if(selected) {
      //draw a white triangle on our selected icon
      chooser_canvas.beginPath();
      chooser_canvas.fillStyle = '#FFFFFF';
      chooser_canvas.moveTo(150, (40*icon_num)+18);
      chooser_canvas.lineTo(135, (40*icon_num)+8);
      chooser_canvas.lineTo(135, (40*icon_num)+28);
      chooser_canvas.closePath();
      chooser_canvas.fill();
    }

    var tab_text = 'icon_'+icon_num;
    chooser_canvas.fillStyle = '#FFFFFF';

    if(!is_new) {
      //this is an existing icon, render the passed image
      chooser_canvas.drawImage(this_img, 4, (40*icon_num)+2);
    } else {
      //this is a new icon, make a + icon and "add new" text
      chooser_canvas.strokeStyle = '#FFFFFF';
      chooser_canvas.lineWidth = 5;
      chooser_canvas.beginPath();
      chooser_canvas.moveTo(4,(40*icon_num)+18);
      chooser_canvas.lineTo(32,(40*icon_num)+18);
      chooser_canvas.moveTo(18, (40*icon_num)+4);
      chooser_canvas.lineTo(18, (40*icon_num)+32);
      chooser_canvas.stroke();

      tab_text = '';
    }

    chooser_canvas.fillText(tab_text, 125-chooser_canvas.measureText(tab_text).width, (40*icon_num)+24);

  }

})(jQuery, Drupal, this, this.document);
