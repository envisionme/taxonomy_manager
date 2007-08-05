
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    //use tree settings....
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        var ul = $('#'+ settings['id']).find("ul");
        Drupal.attachTermData(ul);
      }
      else {
        for (var i=0; i<settings['id'].length; i++) {
          var ul = $('#'+ settings['id'][i]).find("ul");
          Drupal.attachTermData(ul);
        }
      }
    }
  });
}

Drupal.attachTermData = function(ul) {
  $('a.term-data-link').click(function() {
    var tid = Drupal.getTermId($(this).parents("li"));
    Drupal.termDataLoad(this.href, tid);
    return false;
  });
}

Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var tid = Drupal.getTermId($(this).parents("li"));
    Drupal.termDataLoad(this.href, tid);
    return false;
  });
}

Drupal.termDataLoad = function(href, tid) {
  var url = href +'/true';
  $.post(url, null, function(data) {
    var div = $('#taxonomy-term-data');
    $(div).html(data);
    Drupal.autocompleteAutoAttach();
    Drupal.termDataForm(tid, div, href);
  });
}

Drupal.termDataForm = function(tid, div, href) {
  var param = new Object();
  param['tid'] = tid;
  
  $('.term-data-autocomplete-add').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    param['value'] = $(this).parent().find('input:text').attr('value');
    param['op'] = 'add';
    Drupal.termDataSend(param);
    Drupal.termDataLoad(href, tid);
  });
  
  $('.taxonomy-term-data-operations').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    var value = $(this).siblings(".taxonomy-term-data-name").attr("id");
    param['value'] = value.substring(5);
    param['op'] = 'delete';
    $(this).parent().remove();
    Drupal.termDataSend(param);
  });
  
  $('#term-data-name-save').click(function() {
    param['attr_type'] = 'name';
    param['value'] = $('#term-data-name-field').find('input:text').attr('value');
    Drupal.updateTermName(tid, param['value']);
    param['op'] = 'update';
    Drupal.termDataSend(param);
  });
  
  $('#term-data-description-save').click(function() {
    param['value'] = $('#term-data-description-field').find('textarea').attr('value');
    param['attr_type'] = 'description';
    param['op'] = 'update';
    Drupal.termDataSend(param);
  });
  

}

Drupal.termDataSend = function(param) {
  var url= Drupal.settings.termData['url'];
  if (param['value'] != '' && param['attr_type'] != '') {
    $.post(url, param);
  }
}

Drupal.updateTermName = function(tid, name) {
  $('fieldset#taxonomy-term-data-fieldset legend').html(name);
  $('ul.treeview li input:hidden[@class=term-id][@value='+ tid +']')
    .siblings('div.term-item')
    .find('div.form-item label.option a').html(name);
}
