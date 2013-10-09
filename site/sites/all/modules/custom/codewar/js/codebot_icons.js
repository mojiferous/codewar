/**
 * codebot_icons.js
 * Defines codebot icon editor and controls render
 * 8/23/12 Mojiferous
 */
var hidden_icon;
var has_hidden;
var chooser_canvas;
var icon_canvas;
var tot_icons;

(function ($, Drupal, window, document) {

  has_hidden = false;

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

        tot_icons = $('#icon-hidey-hole').children('img').length;

        draw_chooser_icons();
        draw_icon_canvas();

        $('#chooser-canvas').click(function(event_obj) {
          //the chooser canvas has been clicked

          var pos_select = Math.floor(event_obj.offsetY/26);
          if(pos_select < tot_icons) {
            //if we are clicking on an existing icon, redraw it here
            Drupal.settings.codewar.selected_icon = pos_select;
            draw_chooser_icons();
            draw_icon_canvas();
          } else if(pos_select == tot_icons) {
            //the user has clicked on the add icon

            if(tot_icons < 10) {
              Drupal.settings.codewar.selected_icon = tot_icons;
              tot_icons++;
              draw_chooser_icons();
              draw_icon_canvas();
            }

          }

        });

        $('#icon-canvas').mousedown(function(event_obj) {
          //the icon canvas has been clicked, color the square
          var cur_color = $('#chose-color').spectrum("get").toHexString();

          if(cur_color != null && cur_color != '') {
            //make sure we actually have a color
            var x_val = Math.floor(event_obj.offsetX/11);
            var y_val = Math.floor(event_obj.offsetY/11);

            icon_canvas.fillStyle = cur_color;
            icon_canvas.fillRect((x_val*10)+(x_val+1), (y_val*10)+(y_val+1), 10, 10);
          }



        });

      });

    }
  };

  /**
   * draws the icon associated with Drupal.settings.codewar.selected_icon onto the icon_canvas
   */
  function draw_icon_canvas() {
    icon_canvas.fillStyle = '#999999';
    icon_canvas.fillRect(0, 0, 354, 354);

    icon_canvas.strokeStyle = '#AAAAAA';
    icon_canvas.lineWidth = .5;
    icon_canvas.beginPath();
    for(var n=0; n<33; n++) {
      icon_canvas.moveTo(0, (n*10)+(n+1));
      icon_canvas.lineTo(354, (n*10)+(n+1));
      icon_canvas.moveTo((n*10)+(n+1), 0);
      icon_canvas.lineTo((n*10)+(n+1), 354);
    }
    icon_canvas.stroke();

    if(has_hidden) {
      draw_image_data_on_canvas();
    }

  }

  /**
   * renders all the current chooser icons
   */
  function draw_chooser_icons() {
    var icon_num = 0;

    var hidey_hole = $('#icon-hidey-hole').children('img');

    //add each icon to the icon chooser
    for(var n=0; n<tot_icons; n++) {
      var this_img = null;

      if(hidey_hole[n] != null) {
        //there is an icon in the hidey hole for this icon_num
        var this_img_id = $(hidey_hole[n]).attr('id');
        this_img = document.getElementById(this_img_id);
        //draw the unadulterated image onto the hidden image canvas
        hidden_icon.drawImage(this_img, 0, icon_num*32);
        has_hidden = true;
      } else {
        has_hidden = false;
      }

      //draw the icon
      draw_icon(this_img, icon_num, false);

      icon_num++;
    }

    if(tot_icons < 10) {
      //draws a + icon for new icons, but only if we have less than 10 icons
      draw_icon(null, icon_num, true);
    }

  }

  /**
   * draws the image data from the selected icon onto the canvas
   */
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
      if(this_img != null) {
        chooser_canvas.drawImage(this_img, 0, 0, 32, 32, 4, (26*icon_num)+4, 16, 16);
      }

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
