
/**
 * Double Tree Object
 */
Drupal.DoubleTree = function(tree1, tree2) {
  this.leftTree = tree1;
  this.rightTree = tree2;
  this.selected_terms = new Array();
  this.selected_parents = new Array();
  this.direction = "left-to-right";
  this.url = Drupal.settings.DoubleTree['url'];
  
  this.attachOperations(); 
}


Drupal.DoubleTree.prototype.attachOperations = function() {
  var doubleTree = this;
  $('#taxonomy-manager-double-tree-operations :input').click(function() {
    var button_value = $(this).val();
    
    if (button_value == 'Move right') {
      doubleTree.selected_terms = doubleTree.leftTree.getSelectedTerms();
      doubleTree.selected_parents = doubleTree.rightTree.getSelectedTerms();   
    }
    else if (button_value == 'Move left') {
      doubleTree.direction = "right-to-left";
      doubleTree.selected_parents = doubleTree.leftTree.getSelectedTerms();
      doubleTree.selected_terms = doubleTree.rightTree.getSelectedTerms();
    }
    doubleTree.send();
    return false;
  }); 
}

Drupal.DoubleTree.prototype.send = function() {
  var doubleTree = this;
  var param = new Object();
  param['op'] = 'move';
  
  $(this.selected_parents).each(function() {
    var tid = Drupal.getTermId(this);
    param['selected_parents['+ tid +']'] = tid;
  });
  
  $(this.selected_terms).each(function() {
    var tid = Drupal.getTermId(this);
    param['selected_terms['+ tid +']'] = tid;
    param['selected_terms_parent['+ tid +']'] = Drupal.getParentId(this);
  });
  
  $.ajax({
    data: param, 
    type: "POST", 
    url: this.url,
    dataType: 'json',
    success: function(response, status) {
      doubleTree.showMsg(response.data, response.type);
      if (response.type == "status") {
        doubleTree.updateTrees(); 
      }
    }
  });
}


Drupal.DoubleTree.prototype.updateTrees = function() {
  var doubleTree = this;
  if (this.selected_parents.length == 0) {
    //selected terms might be moved to root level, so update whole tree
    var term_to_expand = Drupal.getTermId(this.selected_terms.shift());
    doubleTree.leftTree.loadRootForm(term_to_expand);
    doubleTree.rightTree.loadRootForm(term_to_expand);
  }
  else {
    //remove selected terms and update all parents
    var parents_to_update = new Object();
    $(this.selected_terms).each(function() {
      var parent_tid = Drupal.getParentId(this);
      parents_to_update[parent_tid] = parent_tid;
    });
    for (var i in parents_to_update) {
      var tid = parents_to_update[i];
      doubleTree.updateTree(tid);
    }
    $(this.selected_parents).each(function() {
      var tid = Drupal.getTermId(this);
      doubleTree.updateTree(tid);
    });
  }
}

Drupal.DoubleTree.prototype.showMsg = function(msg, type) {
  var msg_box = '#double-tree-msg';
  $(msg_box).remove();
  $('#taxonomy-manager').before('<div id="double-tree-msg" class="messages '+ type +'">'+ msg +'   <a href="">Close</a></div>');
  $(msg_box).find("a").click(function() {
    $(this).parent().remove();
    return false;
  });
}

Drupal.DoubleTree.prototype.updateTree = function(tid) {
  var doubleTree = this;
  var left_li = this.leftTree.getLi(tid);
  var right_li = this.rightTree.getLi(tid);
  
  this.leftTree.loadChildForm(left_li, true, function(li) {
    doubleTree.updateLi(li);
  });
  this.rightTree.loadChildForm(right_li, true, function(li) {
    doubleTree.updateLi(li);
  });  
}

/**
 * fixes displaying of li node after loading child form
 */
Drupal.DoubleTree.prototype.updateLi = function(li) {
  var children = $(li).children("ul");
  if (children.text() == "") {
    $(li).attr("class", "");
    $(li).children("div.hitArea").remove(); 
  }
  else if ($(li).hasClass("expandable") || $(li).hasClass("lastExpandable")) {
    $(children).hide(); 
  }
  else if (!$(li).hasClass("collapsable") && !$(li).hasClass("lastCollapsable")) {
    $(li).prepend('<div class="hitArea" />');
    $(li).addClass("collapsable");
  }
}


