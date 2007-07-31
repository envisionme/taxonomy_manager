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


Drupal.loadChildForm = function(li) {
  if ($(li).is(".has-children")) {
    var parentId = Drupal.getTermId(li);
    var url = Drupal.settings.childForm['url'];
    url += '/'+ Drupal.getTreeId() +'/'+ parentId;
    $.post(url, null, function(data) {
      $(li).find("ul").remove();
      $(li).find("div.term-line").after(data);
      Drupal.attachTreeview($(li).find("ul"));
      Drupal.attachSiblingsForm();
      Drupal.attachUpdateWeightTerms(li);
      Drupal.attachChildForm(li);
      Drupal.attachTermData($(li).find("ul"))
      $(li).removeClass("has-children");
    });     
  }
}
