
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
    Drupal.TermDataLoad(this);
    return false;
  });
}

Drupal.attachTermDataToSiblings = function(all, currentIndex) {
  var nextSiblings = all.gt(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    Drupal.TermDataLoad(this);
    return false;
  });
}

Drupal.TermDataLoad = function(a) {
  var url = a.href +'/true';
  $.post(url, null, function(data) {
    $('#taxonomy-term-data').html(data);
  });
}
