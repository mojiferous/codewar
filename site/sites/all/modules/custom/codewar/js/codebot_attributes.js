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

        $('#edit-submit').button();
        $('#edit-life-level').buttonset().change(function() {total_values()});
        $('#edit-speed-level').buttonset().change(function() {total_values()});
        $('#edit-ranged-weapon').button().change(function() {total_values()});
        $('#edit-short-weapon').button();
        $('#edit-mid-weapon').button().change(function() {total_values()});
        $('#edit-mine-weapon').button().change(function() {total_values()});
        $('#edit-bomb-weapon').button().change(function() {total_values()});

        //handler for the armor level slider
        $('#armor-slider').slider({
          value: $('#edit-armor-level').val(),
          min: 0,
          max: 10,
          slide: function(event, ui) {
            $('#edit-armor-level').val(ui.value);
            total_values();
          }
        });
        //change val for input change
        $('#edit-armor-level').change(function() {
          var cur_val = $('#armor-slider').slider("value");
          if(cur_val != $(this).val()) {
            $('#armor-slider').slider("value", $(this).val());
          }
          total_values();
        });

        //run through the total_values function on first load, to color value text if necessary
        total_values();

      });

    }
  };

  /**
   * calculate the total value of the form -- the number of points assigned to the bot with the current loadout
   */
  function total_values() {
    //our default point vals are set in php
    var form_vals = Drupal.settings.codewar_vars;

    if(form_vals && form_vals.life_points && form_vals.armor_points && form_vals.speed_points && form_vals.ranged_points && form_vals.mid_points && form_vals.mine_points && form_vals.bomb_points && form_vals.max_points) {
      //convoluted sanity check to prevent NaN issues and broken JS if the settings didn't get properly applied

      var tot_val = $('input[name=life_level]:checked').val()*form_vals.life_points;
      tot_val += $('#edit-armor-level').val()*form_vals.armor_points;
      tot_val += $('input[name=speed_level]:checked').val()*form_vals.speed_points;
      //by doing || 0 here and parsing to int, we always get a number to multiply by, instead of NaN
      tot_val += parseInt($('input[name=ranged_weapon]:checked').val() || 0)*form_vals.ranged_points;
      tot_val += parseInt($('input[name=mid_weapon]:checked').val() || 0)*form_vals.mid_points;
      tot_val += parseInt($('input[name=mine_weapon]:checked').val() || 0)*form_vals.mine_points;
      tot_val += parseInt($('input[name=bomb_weapon]:checked').val() || 0)*form_vals.bomb_points;

      if(tot_val > form_vals.max_points) {
        //the number of points is greater than the maximum allowed points for this weight class
        tot_val = '<span class="error">'+tot_val+'</span>';
      }
      $('#total-points').html(tot_val);
    }

  }

})(jQuery, Drupal, this, this.document);
