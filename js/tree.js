// $Id$


if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        Drupal.attachTreeview(settings['id']);
      }
      else {
        for (var i=0; i<settings['id'].length; i++) {
          Drupal.attachTreeview(settings['id'][i]);
        }
      }
    }
  });
  
}

Drupal.attachTreeview = function(id) {
  $("#"+ id).find("ul")
    .addClass("treeview")
    .find("li:last-child").addClass("last").end()
    .find("li[ul]:last-child").removeClass("last").addClass("lastExpandable").end()	  
    .find("li[ul]").prepend("<div class='hitArea'/>").addClass("expandable").find("ul").hide().end()
    .find("div.hitArea").toggle(
      function() {
        Drupal.toggleTree(this);
      },
      function() {
        Drupal.toggleTree(this);
      }
	);
}

Drupal.toggleTree = function(node) {
  $(node).parent().find("ul:first").toggle();
  Drupal.swapClasses(node.parentNode, "expandable", "collapsable");
  Drupal.swapClasses(node.parentNode, "lastExpandable", "lastCollapsable");
}

Drupal.swapClasses = function(node, c1, c2) {
  if ($.className.has(node, c1)) {
    $(node).removeClass(c1).addClass(c2);
  } 
  else if ($.className.has(node, c2)) {
    $(node).removeClass(c2).addClass(c1);
  } 
}
