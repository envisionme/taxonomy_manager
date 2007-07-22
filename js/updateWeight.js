// $Id$

var weights = new Object();

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    var settings = Drupal.settings.updateWeight;
    Drupal.attachUpdateWeightToolbar(settings['up'], settings['down']);
    Drupal.attachUpdateWeightTerms();     
  });
}

Drupal.attachUpdateWeightToolbar = function(upButton, downButton) {
  var selected;
  var url = Drupal.settings.updateWeight['url'];  
  
  $('#'+ upButton).click(function() {
    selected = Drupal.getSelectedTerms();
    for (var i=0; i < selected.length; i++) {
      var upTerm = selected[i];
      var downTerm = $(upTerm).prev(); 
    
      Drupal.orderTerms(upTerm, downTerm);
    }
    if (selected.length > 0) {
      $.post(url, weights);
    }
  });
  
  
  $('#'+ downButton).click(function() {
    selected = Drupal.getSelectedTerms();
    for (var i=selected.length-1; i >= 0; i--) {
      var downTerm = selected[i];
      var upTerm = $(downTerm).next();
      
      Drupal.orderTerms(upTerm, downTerm);
    }
    if (selected.length > 0) {
      $.post(url, weights);
    }
  });
  
  $('<div><img src="'+ Drupal.settings.updateWeight['modulePath'] +'images/ajax-loader.gif" alt="" height="25"></div>').hide()
    .ajaxStart(function(){
      $(this).show();
    })
    .ajaxStop(function(){
      $(this).hide();
    })
    .appendTo("#taxonomy-toolbar-throbber"); 
}

Drupal.attachUpdateWeightTerms = function(parent) {
  var url = Drupal.settings.updateWeight['url'];
  
  var termArrowsClass = '.term-arrows';
  var termLineClass = '.term-line';
  var termUpClass = '.term-up';
  var termDownClass = '.term-down';
  
  if (parent) {
    termArrowsClass = $(parent).find(termArrowsClass);
    termLineClass = $(parent).find(termLineClass);
    termUpClass = $(parent).find(termUpClass);
    termDownClass = $(parent).find(termDownClass);
  }
  
  $(termArrowsClass).hide();
  
  $(termLineClass).mouseover(function() {
    $(this).find('.term-arrows').show();
  });
  
  $(termLineClass).mouseout(function() {
    $(this).find('.term-arrows').hide();
  });
  
  
  $(termUpClass).click(function() {
    var upTerm = $(this).parents("li").eq(0);
    var downTerm = $(upTerm).prev(); 
    
    Drupal.orderTerms(upTerm, downTerm);
    $(downTerm).find('.term-arrows').hide();
    $(upTerm).find('.term-arrows').hide();
    $.post(url, weights);
  });
  
  
  $(termDownClass).click(function() {
    var downTerm = $(this).parents("li").eq(0);
    var upTerm = $(downTerm).next();
    
    Drupal.orderTerms(upTerm, downTerm);
    $(downTerm).find('.term-arrows').hide();
    $(upTerm).find('.term-arrows').hide();
    $.post(url, weights);
  });

}

Drupal.getSelectedTerms = function() {
  var terms = new Array();
  $('.treeview').find("input[@type=checkbox][@checked]").each(function() {
    var term = $(this).parents("li").eq(0);
    terms.push(term);
  });
  
  return terms;
}

Drupal.orderTerms = function(upTerm, downTerm) {
  try {
    Drupal.getTermId(upTerm);
    Drupal.swapTerms(upTerm, downTerm);
    Drupal.swapWeights(upTerm, downTerm);
    Drupal.updateTree(upTerm, downTerm);
  } catch(e) {
    //no next item, because term to update is last child, continue
  }
}

Drupal.swapTerms = function(upTerm, downTerm) { 
  $(upTerm).after(downTerm);
  $(downTerm).before(upTerm);
}

Drupal.swapWeights = function(upTerm, downTerm) {
  var upWeight = Drupal.getWeight(upTerm);
  var downWeight = Drupal.getWeight(downTerm);
  var downTid = Drupal.getTermId(downTerm);
  var upTid = Drupal.getTermId(upTerm);
  
  //same weight, decrease upTerm
  if (upWeight == downWeight) {
    weights[upTid] = --upWeight;
  }
  //different weights, swap
  else {
    weights[upTid] = downWeight;
    weights[downTid] = upWeight;
  }
  
  
  var updateUpTerms = false;
  var updateDownTerms = false;
  
  try {
    if (Drupal.getWeight($(upTerm).prev()) == upWeight) {
      updateUpTerms = true;
    }
  } catch(e) {
    //no prev
  }
  
  try {
    if (Drupal.getWeight($(upTerm).next()) == downWeight) {
      updateDownTerms = true;
    }
  } catch(e) {
    //no next
  }
  
  //in/decrease all siblings if necessary
  if (updateUpTerms || updateDownTerms) {
    $(upTerm).siblings().each(function() {
      var id = Drupal.getTermId(this);
      if (id != downTid) {
        var weight = Drupal.getWeight(this);
        if (weight <= upWeight && updateUpTerms) {
          weights[id] = --weight;
        }
        else if (weight >= downWeight && updateDownTerms) {
          weights[id] = ++weight;
        }
      }
    });
  }
}

Drupal.getWeight = function(li) {
  var id = Drupal.getTermId(li);
  var weight;
  
  if (weights[id] != null) {
    weight = weights[id];
  }
  else {
    weight = $(li).find("input:hidden[@class=weight-form]").attr("value");
  }
  
  return weight;
}

Drupal.getTermId = function(li) {
  var id = $(li).find("input:hidden[@class=term-id]").attr("value");
  return id;
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

