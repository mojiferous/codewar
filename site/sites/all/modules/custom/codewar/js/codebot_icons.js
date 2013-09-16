/**
 * codebot_icons.js
 * Defines codebot icon editor and controls render
 * 8/23/12 Mojiferous
 */
var hidden_icon;
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
        hidden_icon = document.getElementById('hidden-icon').getContext('2d');
        chooser_canvas = document.getElementById('chooser-canvas').getContext('2d');
        icon_canvas = document.getElementById('icon-canvas').getContext('2d');

        var icon_num = 0;

        //add each icon to the icon chooser
        $('#icon-hidey-hole').children('img').each(function() {
          var this_img_id = $(this).attr('id');
          var this_img = document.getElementById(this_img_id);

          //draw the unadulterated image onto the hidden image canvas
          hidden_icon.drawImage(this_img, 0, icon_num*32);

          //draw the icon
          draw_icon(this_img, icon_num, false);

          icon_num++;
        });

        //draws a + icon for new icons
        draw_icon(null, icon_num, true);

        tot_icons = icon_num-1;

        icon_canvas.fillStyle = '#999999';
        icon_canvas.fillRect(0, 0, 354, 354);

        icon_canvas.strokeStyle = '#AAAAAA';
        icon_canvas.lineWidth = .5;
        icon_canvas.beginPath();
        for(var n=0; n<33; n++) {
          icon_canvas.moveTo(0, n*10+1);
          icon_canvas.lineTo(354, n*10+1);
          icon_canvas.moveTo(n*10+1, 0);
          icon_canvas.lineTo(n*10+1, 354);
        }
        icon_canvas.stroke();

        draw_image_data_on_canvas();

      });

    }
  };

  function draw_image_data_on_canvas() {
    for(var x=0; x<32; x++) {
      for(var y=0; y<32; y++) {
        var this_data = hidden_icon.getImageData(x, (Drupal.settings.codewar.selected_icon*32)+y, 1, 1);
        var this_color = 'rgb('+this_data.data[0]+','+this_data.data[1]+','+this_data.data[2]+')';
        icon_canvas.fillStyle = this_color;
        icon_canvas.fillRect((x*10)+(x+1), (y*10)+(y+1), 10, 10);
      }
    }
  }

  /**
   * draws an icon at icon_num location, uses the Drupal.settings object to determine the currently selected icon
   * @param this_img
   * @param icon_num
   * @param is_new
   */
  function draw_icon(this_img, icon_num, is_new) {
    var selected = (Drupal.settings.codewar.selected_icon == icon_num);
    chooser_canvas.font = '12px Arial';
    chooser_canvas.fillStyle = '#CCCCCC';

    if(selected) {
      //this is our selected icon, change the tab color
      chooser_canvas.fillStyle = '#999999';
    }

    chooser_canvas.fillRect(0, (26*icon_num), 100, 24);

    if(selected) {
      //draw a white triangle on our selected icon
//      chooser_canvas.beginPath();
//      chooser_canvas.fillStyle = '#FFFFFF';
//      chooser_canvas.moveTo(100, (26*icon_num)+13);
//      chooser_canvas.lineTo(85, (26*icon_num)+4);
//      chooser_canvas.lineTo(85, (26*icon_num)+20);
//      chooser_canvas.closePath();
//      chooser_canvas.fill();
    }

    var tab_text = 'icon_'+icon_num;
    chooser_canvas.fillStyle = '#FFFFFF';

    if(!is_new) {
      //this is an existing icon, render the passed image
      chooser_canvas.drawImage(this_img, 0, 0, 32, 32, 4, (26*icon_num)+4, 16, 16);
    } else {
      //this is a new icon, make a + icon and "add new" text
      chooser_canvas.strokeStyle = '#FFFFFF';
      chooser_canvas.lineWidth = 3;
      chooser_canvas.beginPath();
      chooser_canvas.moveTo(4,(26*icon_num)+12);
      chooser_canvas.lineTo(20,(26*icon_num)+12);
      chooser_canvas.moveTo(12, (26*icon_num)+4);
      chooser_canvas.lineTo(12, (26*icon_num)+20);
      chooser_canvas.stroke();

      tab_text = '';
    }

    chooser_canvas.fillText(tab_text, 24, (26*icon_num)+16);

  }

})(jQuery, Drupal, this, this.document);
