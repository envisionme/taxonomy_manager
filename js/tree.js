// $Id$


if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        var ul = $('#'+ settings['id']).find("ul");
        Drupal.attachTreeview(ul);
        Drupal.attachThrobber(settings['id']);
      }
      else {
        for (var i=0; i<settings['id'].length; i++) {
          var ul = $('#'+ settings['id'][i]).find("ul");
          Drupal.attachTreeview(ul);
          Drupal.attachThrobber(settings['id'][i]);
        }
      }
    }
  });
}

Drupal.attachTreeview = function(ul) {
  $(ul)
    .addClass("treeview")
    .find("li[ul]").prepend("<div class='hitArea'/>").find("ul").hide().end()
    .find("div.hitArea").click(function() {
      Drupal.toggleTree(this);
    });
}

Drupal.attachTreeviewToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  nextSiblings.children("ul").each(function() {
    var ul_nested = $(this);
    var li = $(ul_nested).parent();
    $(li).prepend("<div class='hitArea'/>");
    $(ul_nested).hide();
    $(li).find("div.hitArea").click(function() {
      Drupal.toggleTree(this);
    });
  });
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

Drupal.getTermId = function(li) {
  var id = $(li).find("input:hidden[@class=term-id]").attr("value");
  return id;
}

Drupal.getParentId = function(li) {
  var parentId;
  try {
    var parentLi = $(li).parent("ul").parent("li");
    parentId = Drupal.getTermId(parentLi);
  } catch(e) {
    return 0;
  }
  return parentId;
}

Drupal.updateTree = function(upTerm, downTerm) {  
  if ($(upTerm).is(".last")) {
    $(upTerm).removeClass("last");
    Drupal.updateTreeDownTerm(downTerm); 
  }
  else if ($(upTerm).is(".lastExpandable")) {
    $(upTerm).removeClass("lastExpandable").addClass("expandable");
    Drupal.updateTreeDownTerm(downTerm); 
  }
  else if ($(upTerm).is(".lastCollapsable")) {
    $(upTerm).removeClass("lastCollapsable").addClass("collapsable");
    Drupal.updateTreeDownTerm(downTerm);  
  }
}

Drupal.updateTreeDownTerm = function(downTerm) {
  if ($(downTerm).is(".expandable")) {
    $(downTerm).removeClass("expandable").addClass("lastExpandable");
  }
  else if ($(downTerm).is(".collapsable")) {
    $(downTerm).removeClass("collapsable").addClass("lastCollapsable");
  }
  else {
    $(downTerm).addClass("last");
  }
}

Drupal.getTreeId = function() {
  return Drupal.settings.taxonomytree['id']; 
}

Drupal.attachThrobber = function(tree) {
  $('<div><img src="'+ Drupal.settings.taxonomy_manager['modulePath'] +'images/ajax-loader.gif" alt="" height="25"></div>').hide()
    .ajaxStart(function(){
      $(this).show();
      $('#'+ tree).css('opacity', '0.5');
    })
    .ajaxStop(function(){
      $(this).hide();
      $('#'+ tree).css('opacity', '1');
    })
    .appendTo("#taxonomy-toolbar-throbber"); 
}
