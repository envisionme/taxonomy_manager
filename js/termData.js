// $Id$

/**
 * @file js support for term editing form for ajax saving and tree updating
 */

Drupal.behaviors.TaxonomyManagerTermData = function(context) {
  //use tree settings....
  var settings = Drupal.settings.taxonomytree || [];
  if (settings['id']) {
    if (!(settings['id'] instanceof Array)) {
      if (Drupal.settings.termData['tid']) {
        var termdata = new Drupal.TermData(Drupal.settings.termData['tid'], Drupal.settings.termData['term_url']);
        termdata.form();
      }
      var ul = $('#'+ settings['id']).find("ul");
      Drupal.attachTermData(ul);
    }
  }
}

/**
 * adds click events to the term links in the tree structure
 */
Drupal.attachTermData = function(ul) {
  $(ul).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href, li);
    termdata.load();
    return false;
  });
}

/**
 * attaches click events to next siblings
 */
Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = $(all).slice(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href, li);
    termdata.load();
    return false;
  });
}

/**
 * TermData Object
 */
Drupal.TermData = function(tid, href, li) {
  this.href = href;
  this.tid = tid;
  this.li = li;
}


/**
 * loads ahah form from given link and displays it on the right side
 */
Drupal.TermData.prototype.load = function() {
  var url = this.href +'/true';
  var termdata = this;
  $.get(url, null, function(data) {
    termdata.div = $('#taxonomy-term-data');
    $(termdata.div).html(data);
    termdata.form();
  });
}

/**
 * adds events to possible operations
 */
Drupal.TermData.prototype.form = function() {
  var termdata = this;
  this.param = new Object();

  try {
    Drupal.behaviors.textarea(this.div);
    Drupal.behaviors.autocomplete(this.div);
  } catch(e) {} //autocomplete or textarea js not added to page
  
  this.param['tid'] = this.tid;
  
  $('.term-data-autocomplete-add').click(function() {
    termdata.param['attr_type'] = $(this).find("img").attr("class");
    termdata.param['value'] = $(this).parent().find('input:text').attr('value');
    termdata.param['op'] = 'add';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
  });
  
  $('.taxonomy-term-data-operations').click(function() {
    termdata.param['attr_type'] = $(this).find("img").attr("class");
    termdata.param['info'] = $(this).find("img").attr("id");
    var value = $(this).siblings(".taxonomy-term-data-name").attr("id");
    termdata.param['value'] = value.substring(5);
    termdata.param['op'] = 'delete';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
    $(this).parent().remove();
  });
  
  $('#term-data-name-save').click(function() {
    termdata.param['attr_type'] = 'name';
    termdata.param['value'] = $('#term-data-name-field').find('input:text').attr('value');
    termdata.param['op'] = 'update';
    termdata.send();
    termdata.updateTermName();
  });
  
  $('#term-data-description-save').click(function() {
    termdata.param['value'] = $('#term-data-description-field').find('textarea').attr('value');
    termdata.param['attr_type'] = 'description';
    termdata.param['op'] = 'update';
    termdata.send();
  });
  
  $('#edit-term-data-weight').change(function() {
    termdata.param['value'] = this.value;
    termdata.param['attr_type'] = 'weight';
    termdata.param['op'] = 'update';
    termdata.send();
  });

}

/**
 * sends updated data through param
 */
Drupal.TermData.prototype.send = function() {
  var termdata = this;
  var url= Drupal.settings.termData['url'];
  if (this.param['value'] != '' && this.param['attr_type'] != '') {
    $.ajax({
      data: termdata.param, 
      type: "POST", 
      url: url,
      complete: function() {
        termdata.update();
      }
    });
  }
}

/**
 * updates term data form and if necessary tree structure
 */
Drupal.TermData.prototype.update = function() {
  if (this.param['op'] == 'add') {
    this.load();
  }
  var tree = new Drupal.TaxonomyManagerTree($("div#taxonomy-manager-tree"));
 
  if (this.param['attr_type'] == 'parent' || (this.param['attr_type'] == 'related' && this.param['op'] == 'add')) {
    tree.loadRootForm();
  }
  else if (this.param['attr_type'] == 'weight') {
    var parentLi = $(this.li).parents("li");
    if ($(parentLi).is("li")) {
      tree.loadChildForm(parentLi, true);
    }
    else {
      //no parent li --> root level terms
      //load whole Tree
      tree.loadRootForm();
    }
  }
}

/**
 * updates term name in tree structure
 */
Drupal.TermData.prototype.updateTermName = function() {
  var name = this.param['value'];
  $('fieldset#taxonomy-term-data-fieldset legend').html(name);
  $('ul.treeview li input:hidden[@class=term-id][@value='+ this.tid +']')
    .siblings('div.term-item')
    .find('div.form-item label.option a').html(name);
}
