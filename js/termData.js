// $Id$

/**
 * @file js support for term editing form for ajax saving and tree updating
 */

//global var that holds the current term link object
var active_term = new Object();

Drupal.behaviors.TaxonomyManagerTermData = function(context) {
  //use tree settings....
  var settings = Drupal.settings.taxonomytree || [];
  if (settings['id']) {
    
    if (!(settings['id'] instanceof Array)) {
      if (!$('#taxonomy-manager-toolbar' + '.tm-termData-processed').size()) {
        $('#taxonomy-manager-toolbar').addClass('tm-termData-processed');
        var ul = $('#'+ settings['id']).find("ul");
        Drupal.attachTermData(ul);
      }
      
      var tid = $('#edit-term-data-tid').val();
      if (tid) {
        var url = Drupal.settings.termData['term_url'] +'/'+ tid +'/true';
        var termdata = new Drupal.TermData(tid, url);
        termdata.form();
      }
    }
  }
}

/**
 * adds click events to the term links in the tree structure
 */
Drupal.attachTermData = function(ul) {
  $(ul).find('a.term-data-link').click(function() {
    Drupal.activeTermSwapHighlight(this);
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href +'/true', li);
    termdata.load();
    return false;
  });
}

/**
* hightlights current term
*/
Drupal.activeTermSwapHighlight = function(link) {
  try {
    $(active_term).parents('div.form-item:first').removeClass('highlightActiveTerm');
  } catch(e) {}
  active_term = link;
  $(active_term).parents('div.form-item:first').addClass('highlightActiveTerm');
}

/**
 * attaches click events to next siblings
 */
Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = $(all).slice(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href +'/true', li);
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
  this.form_build_id = $(' :input[name="form_build_id"]').val();
  this.form_id = $(' :input[name="form_id"]').val();
  this.div = $('#taxonomy-term-data');
}


/**
 * loads ahah form from given link and displays it on the right side
 */
Drupal.TermData.prototype.load = function() {
  var url = this.href;
  var termdata = this;
  var param = new Object();
  param['form_build_id'] = this.form_build_id;
  param['form_id'] = this.form_id;
  
  $.get(url, param, function(data) {
    termdata.insertForm(data);
  });
}

/**
 * inserts received html data into form wrapper
 */
Drupal.TermData.prototype.insertForm = function(data) { 
  $(this.div).html(data);
  this.form(); 
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
    Drupal.behaviors.ahah(this.div);
  } catch(e) {} //autocomplete or textarea js not added to page
  
  this.param['tid'] = this.tid;
  this.param['vid'] = $('input#edit-term-data-vid').attr('value');
  
  $(this.div).find('div.term-data-autocomplete-add > span').click(function() {
    termdata.param['attr_type'] = $(this).attr("class");
    termdata.param['value'] = $(this).parents("tr").find('input:text').attr('value');
    termdata.param['op'] = 'add';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
  });
  
  $(this.div).find('td.taxonomy-term-data-operations > span').click(function() {
    termdata.param['attr_type'] = $(this).attr("class");
    termdata.param['info'] = $(this).attr("id");
    var value = $(this).parent().siblings(".taxonomy-term-data-name").attr("id");
    termdata.param['value'] = value.substring(5);
    termdata.param['op'] = 'delete';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
  });
  
  $(this.div).find('#edit-term-data-weight').change(function() {
    termdata.param['value'] = this.value;
    termdata.param['attr_type'] = 'weight';
    termdata.param['op'] = 'update';
    termdata.send();
  });

  $(this.div).find('#edit-term-data-save').click(function() {
    termdata.param['value'] = $('#edit-term-data-name').attr('value');
    termdata.updateTermName();
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
      dataType: 'json',
      success: function(response, status) {
        termdata.update(); 
        termdata.insertForm(response.data);
      }
    });
  }
}

/**
 * updates term data form and if necessary tree structure
 */
Drupal.TermData.prototype.update = function() {
  var settings = Drupal.settings.taxonomytree || [];
  var id, vid;
  
  if (settings['id']) {
    if (!(settings['id'] instanceof Array)) {
       id = settings['id'];
       vid = settings['vid'];
    }
  }
    
  var tree = new Drupal.TaxonomyManagerTree(id, vid);
 
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
  $('ul.treeview li input:hidden[class=term-id][value='+ this.tid +']')
    .siblings('div.form-item')
    .find('label.option a').html(name);
}
