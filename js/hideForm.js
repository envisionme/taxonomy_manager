// $Id$

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.hideForm || [];
    
    if (settings['div']) {
      if (!(settings['div'] instanceof Array)) {
        Drupal.attachHideForm(settings['div'], settings['show_button'], settings['hide_button']);
      }
      else {
        for (var i=0; i<settings['div'].length; i++) {
          Drupal.attachHideForm(settings['div'][i], settings['show_button'][i], settings['hide_button'][i]); 
        }
      }
    }
     
  })
}

Drupal.attachHideForm = function(div, show_button, hide_button) {
  var hide = true;
  div = "#"+ div;
  show_button = "#"+ show_button;
  hide_button = "#"+ hide_button;
  
  $(div).find("input").each(function() {
    if($.className.has(this, "error")) {
      hide = false;
    }
  });
  
  if (hide) { 
    $(div).hide();
  }
  
  $(show_button).click(function() {
    $(div).show();
  });
  
  $(hide_button).click(function() {
    $(div).hide();
  });
}