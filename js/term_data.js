
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    //use tree settings....
    var settings = Drupal.settings.taxonomytree || [];
    if (settings['id']) {
      if (!(settings['id'] instanceof Array)) {
        if (Drupal.settings.termData['tid']) {
          Drupal.termDataForm(Drupal.settings.termData['tid'], Drupal.settings.termData['term_url']);
        }
        else {
          var ul = $('#'+ settings['id']).find("ul");
          Drupal.attachTermData(ul);
        }
      }
      /*else {
        for (var i=0; i<settings['id'].length; i++) {
          var ul = $('#'+ settings['id'][i]).find("ul");
          Drupal.attachTermData(ul);
        }
      }*/
    }
  });
}

Drupal.attachTermData = function(ul) {
  $('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var tid = Drupal.getTermId(li);
    Drupal.termDataLoad(this.href, tid, li);
    return false;
  });
}

Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var tid = Drupal.getTermId(li);
    Drupal.termDataLoad(this.href, tid, li);
    return false;
  });
}

Drupal.termDataLoad = function(href, tid, li) {
  var url = href +'/true';
  $.get(url, null, function(data) {
    var div = $('#taxonomy-term-data');
    $(div).html(data);
    Drupal.termDataForm(tid, href, li);
  });
}

Drupal.termDataForm = function(tid, href, li) {
  try {
    Drupal.autocompleteAutoAttach();
    Drupal.textareaAttach();
  } catch(e) {} //autocomplete or textarea js not added to page
  
  var param = new Object();
  param['tid'] = tid;
  
  $('.term-data-autocomplete-add').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    param['value'] = $(this).parent().find('input:text').attr('value');
    param['op'] = 'add';
    Drupal.termDataSend(param);
    Drupal.termDataLoad(href, tid);
    Drupal.reloadTree(li, tid, param);

  });
  
  $('.taxonomy-term-data-operations').click(function() {
    param['attr_type'] = $(this).find("img").attr("class");
    var value = $(this).siblings(".taxonomy-term-data-name").attr("id");
    param['value'] = value.substring(5);
    param['op'] = 'delete';
    $(this).parent().remove();
    Drupal.termDataSend(param);
    Drupal.reloadTree(li, tid, param);
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
  
  $('#edit-term-data-weight').change(function() {
    param['value'] = this.value;
    param['attr_type'] = 'weight';
    param['op'] = 'update';
    Drupal.termDataSend(param);
    
    Drupal.reloadTree(li, tid, param);
    
  });
}

Drupal.termDataSend = function(param) {
  var url= Drupal.settings.termData['url'];
  if (param['value'] != '' && param['attr_type'] != '') {
    //synchronous to ensure consistent data
    $.ajax({
      async: false,
      data: param, 
      type: "POST", 
      url: url
    });
  }
}

Drupal.reloadTree = function (li, tid, param) {;
  if (param['attr_type'] == 'parent' || (param['attr_type'] == 'child' && param['op'] == 'add')) {
    Drupal.loadRootForm();
  }
  else if (param['attr_type'] == 'child' || param['attr_type'] == 'weight') {
    var parentLi = $(li).parents("li");
    try {
      parentLi.attr("id");
      Drupal.loadChildForm(parentLi, true);
    } catch (e) {
      //no parent li --> root level terms
      //load whole Tree
      Drupal.loadRootForm();
    }
  }
}

Drupal.updateTermName = function(tid, name) {
  $('fieldset#taxonomy-term-data-fieldset legend').html(name);
  $('ul.treeview li input:hidden[@class=term-id][@value='+ tid +']')
    .siblings('div.term-item')
    .find('div.form-item label.option a').html(name);
}