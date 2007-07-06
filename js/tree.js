// $Id$
 /**
 * @author Matthias
 */


if (Drupal.jsEnabled) {
  $(document).ready(function() {	  
    Drupal.attachTreeview();
  });
  
}

Drupal.attachTreeview = function() {
  $("#taxonomytree").find("ul")
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
  //$(node).siblings().find("li[ul]").find("ul").hide();
}

Drupal.swapClasses = function(node, c1, c2) {
  if ($.className.has(node, c1)) {
    $(node).removeClass(c1).addClass(c2);
  } 
  else if ($.className.has(node, c2)) {
    $(node).removeClass(c2).addClass(c1);
  } 
}
