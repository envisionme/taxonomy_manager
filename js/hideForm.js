// $Id$

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.hideForm;

    for (var i=0; i<settings['div'].length; i++) {
      Drupal.attachHideForm(settings['div'][i], settings['show_button'][i], settings['hide_button'][i]); 
    }
     
  })
}

Drupal.attachHideForm = function(div, show_button, hide_button) {
  $("#"+ div).hide();
  $("#"+ show_button).click(function() {
    $("#"+ div).show();
  });
  $("#"+ hide_button).click(function() {
    $("#"+ div).hide();
  });
}