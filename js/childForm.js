// $Id$

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    Drupal.attachChildForm();  
  })
}

Drupal.attachChildForm = function(subTree) {
  var list = "li.has-children div.hitArea";
  if (subTree) {
    list = $(subTree).find(list);
  }
  
  $(list).click(function() {
    Drupal.loadChildForm($(this).parent());
  });
}

Drupal.attachChildFormToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).filter('.has-children').find('div.hitArea').click(function() {
    Drupal.loadChildForm($(this).parent());
  });
}


Drupal.loadChildForm = function(li, update) {
  if ($(li).is(".has-children") || update == true) {
    var parentId = Drupal.getTermId(li);
    var url = Drupal.settings.childForm['url'];
    url += '/'+ Drupal.getTreeId() +'/'+ Drupal.getVocId() +'/'+ parentId;
    $.get(url, null, function(data) {
      $(li).find("ul").remove();
      $(li).find("div.term-line").after(data);
      var ul = $(li).find("ul");
      Drupal.attachTreeview(ul);
      Drupal.attachSiblingsForm(ul);
      Drupal.attachUpdateWeightTerms(li);
      Drupal.attachChildForm(li);
      Drupal.attachTermData($(li).find("ul"));
      $(li).removeClass("has-children");
    });     
  }
}

Drupal.loadRootForm = function() {
  var treeId = Drupal.getTreeId();
  var url = Drupal.settings.childForm['url'];
  url += '/'+ treeId +'/'+ Drupal.getVocId() +'/0/true';
  $.get(url, null, function(data) {
    $('#'+ treeId).html(data);
    var ul = $('#'+ treeId).find("ul");
    Drupal.attachTreeview(ul);
    Drupal.attachSiblingsForm();
    Drupal.attachUpdateWeightTerms();
    Drupal.attachChildForm();
    Drupal.attachTermData();
    
  });
}


