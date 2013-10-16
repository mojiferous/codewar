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

      });

    }
  };

  function total_values() {
    var tot_val = $('input[name=life_level]:checked').val()*5;
    tot_val += $('#edit-armor-level').val()*2;
    tot_val += $('input[name=speed_level]:checked').val()*5;
    //by doing || 0 here and parsing to int, we always get a number to multiply by, instead of NaN
    tot_val += parseInt($('input[name=ranged_weapon]:checked').val() || 0)*10;
    tot_val += parseInt($('input[name=mid_weapon]:checked').val() || 0)*2;
    tot_val += parseInt($('input[name=mine_weapon]:checked').val() || 0)*10;
    tot_val += parseInt($('input[name=bomb_weapon]:checked').val() || 0)*10;

    $('#total-points').html(tot_val);
  }

})(jQuery, Drupal, this, this.document);
