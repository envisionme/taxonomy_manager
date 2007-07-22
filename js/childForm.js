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
    var li = $(this).parent();
    
    if ($(li).is(".has-children")) {
      var parentId = Drupal.getTermId(li);
      var url = Drupal.settings.childForm['url'];
      url += '/'+ parentId;
      $.post(url, null, function(data) {
        $(li).find("ul").remove();
        $(li).find("div.term-line").after(data);
        Drupal.attachTreeview($(li).find("ul"));
        Drupal.attachUpdateWeightTerms(li);
        Drupal.attachChildForm(li);
        $(li).removeClass("has-children");
      });     
    }
  });
}

