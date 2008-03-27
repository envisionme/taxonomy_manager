<?php


/**
 * list of vocabularies, which link to Taxonomy Manager interface
 */
function taxonomy_manager_voc_list() {
  $vocabularies = taxonomy_get_vocabularies();
  $voc_list = array();
  
  foreach ($vocabularies as $vocabulary) {
    $voc_list[] = l($vocabulary->name, 'admin/content/taxonomy_manager/voc/'. $vocabulary->vid);
  }
  
  return theme('item_list', $voc_list, t("Vocabularies:"));
}

/**
 * defines forms for taxonomy manager interface
 * 
 * @param $vid vocabulary id
 * @param $tid a term id, if not 0, displays term editing from for given tid on right side
 * @param $search_string a string to filter root level terms
 */
function taxonomy_manager_form($form_state, $vid, $tid = 0, $filter = NULL) {
  $module_path = drupal_get_path('module', 'taxonomy_manager') .'/';
  drupal_add_css($module_path .'css/taxonomy_manager.css');
  drupal_add_js($module_path .'js/hideForm.js');
  drupal_add_js($module_path .'js/updateWeight.js');
  drupal_add_js($module_path .'js/termData.js');
  drupal_add_js('misc/textarea.js');  //because of term data form wich gets dynamically loaded
  
  drupal_add_js(array('termData' => array('url' => url("admin/content/taxonomy_manager/termdata/edit/". $vid), 'tid' => $tid, 'term_url' => url(taxonomy_manager_tree_link($vid) .'/'. $tid .'/true'))), 'setting');
  drupal_add_js(array('updateWeight' => array('up' => 'edit-weight-up', 'down' => 'edit-weight-down', 'url' => url('admin/content/taxonomy_manager/weight/'. $vid), 'disable_mouseover' => variable_get('taxonomy_manager_disable_mouseover_weights', 0))), 'setting');
  
  $form = array();

  $voc = taxonomy_vocabulary_load($vid);
  
  drupal_set_title(t("Taxonomy Manager - %voc_name", array("%voc_name" => $voc->name)));
  
  if (!is_numeric($voc->vid)) {
    $text = t('No vocabulary with this ID available!. ');
    $text .= t('Check this !link_list for available vocabularies or !link_create a new one', array('!link_list' => l('list', 'admin/content/taxonomy_manager'), '!link_create' => l('create', 'admin/content/taxonomy/add/vocabulary')));
    $form['text'] = array( 
      '#value' => $text,
    );
    return $form;
  }
  
  $form['vid'] = array('#type' => 'value', "#value" => $vid);
  
  if (_taxonomy_manager_voc_is_empty($vid)) {
    $text = t('No terms available');
    $form['text'] = array( 
      '#value' => $text,
    );
    $form += taxonomy_manager_add_form($voc, false);
    return $form;
  }
  
  $form['#cache'] = TRUE;
  
  $form['taxonomy']['#tree'] = TRUE;
  
  $form['taxonomy']['manager'] = array( 
    '#type' => 'fieldset',
    '#title' => $voc->name,
    '#weight' => 10,
    '#tree' => TRUE,
    '#prefix' => '<div id="taxonomy-manager-tree-outer-div">',
    '#suffix' => '</div>',
  );
  
  $form['taxonomy']['manager']['tree'] = array( 
    '#type' => 'taxonomy_manager_tree', 
    '#vid' => $vid, 
    '#pager' => TRUE,
    '#search_string' => ($tid) ? NULL : $filter,
  );
  
  $search_description = t("You can search directly for exisiting terms. 
      If your input doesn't match an existing term, it will be used for filtering root level terms (useful for non-hierarchical vocabularies).");
  
  $form['search'] = array(  
    '#type' => 'fieldset',
    '#attributes' => array('id' => 'taxonomy-manager-search'), 
    '#title' => t('Search'),
    '#description' => $search_description, 
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
    '#tree' => TRUE, 
  );
  
  $form['search']['field'] = array( 
    '#type' => 'textfield', 
    '#title' => t('Search String'), 
    '#autocomplete_path' => 'taxonomy/autocomplete/'. $voc->vid,
    '#prefix' => '<div id="edit-find-field">', 
    '#suffix' => '</div>', 
  );
  
  $form['search']['button'] = array( 
    '#type' => 'submit', 
    '#attributes' => array('class' => 'taxonomy-manager-buttons search'),
    '#value' => t('Search'),
    '#suffix' => '<div class="clear"></div>',
  );
 
  $form['toolbar'] = array( 
    '#type' => 'fieldset', 
    '#title' => t('Toolbar'),
    '#attributes' => array('id' => 'taxonomy-manager-toolbar'),
  );
  
  $form['toolbar']['weight_up'] = array( 
    '#type' => 'button',
    '#attributes' => array('class' => 'taxonomy-manager-buttons'), 
    '#value' => t('Up'),
    '#theme' => 'no_submit_button',
    '#prefix' => '<div id="taxonomy-manager-toolbar-buttons">',
  );
  
  $form['toolbar']['weight-down'] = array( 
    '#type' => 'button', 
    '#attributes' => array('class' => 'taxonomy-manager-buttons'),
    '#value' => t('Down'),
    '#theme' => 'no_submit_button',
  );
   
  $form['toolbar']['delete_confirm'] = array( 
    '#type' => 'button', 
    '#attributes' => array('class' => 'taxonomy-manager-buttons delete'),
    '#value' => t('Delete'),
    '#theme' => 'no_submit_button',
  );

  $form['toolbar']['add_show'] = array( 
    '#type' => 'button',
    '#attributes' => array('class' => 'taxonomy-manager-buttons add'), 
    '#value' => t('Add'), 
    '#theme' => 'no_submit_button',
  );
  
  //moving is only possible in hierarchies...
  if ($voc->hierarchy != 0) {
    $form['toolbar']['move_show'] = array( 
      '#type' => 'button', 
      '#value' => t('Move'),
      '#attributes' => array('class' => 'taxonomy-manager-buttons move'),
       '#theme' => 'no_submit_button', 
    );
  }
  
  $form['toolbar']['merge_show'] = array( 
    '#type' => 'button', 
    '#attributes' => array('class' => 'taxonomy-manager-buttons merge'),
    '#value' => t('Merge'), 
    '#theme' => 'no_submit_button', 
    '#suffix' => '</div>'
  );
  
  $form['toolbar']['wrapper'] = array( 
    '#type' => 'markup', 
    '#value' => '<div id="taxonomy-manager-toolbar-throbber"></div><div class="clear"></div>', 
    '#weight' => 20,
  );
  
  $form += taxonomy_manager_add_form($voc);
  
  $form += taxonomy_manager_merge_form($voc);
  
  $form += taxonomy_manager_move_form($voc);
  
  $form += taxonomy_manager_confirm_delete($voc);
  
  if ($tid) {
    $form += taxonomy_manager_form_term_data($tid);
  }
  
  return $form;
}

/**
 * confirmation form for deleting selected terms
 */
function taxonomy_manager_confirm_delete($voc) {
  drupal_add_js(array('hideForm' => array(
    'show_button' => 'edit-delete-confirm', 
    'hide_button' => 'edit-delete-cancel', 
    'div' => 'del-confirm-form')), 'setting');
    
  $form = array();
  
  $form['delete'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Confirmation'),
    '#prefix' => '<div id="del-confirm-form">',
    '#suffix' => '</div>',
  );
  
  $question = t('Are you sure you want to delete all selected terms? ');
  $info = t('Remember all term specific data will be lost. This action cannot be undone.');
  
  $form['delete']['text'] = array('#value' => "<b>". $question ."</b><br/>". $info);
  
  if ($voc->hierarchy != 0) {
    $options = array( 
      'delete_orphans' => t('Delete children of selected terms, if there are any'), 
    );
     
    $form['delete']['options'] = array( 
      '#type' => 'checkboxes', 
      '#title' => t('Options'), 
      '#options' => $options, 
    );
  }
  
  $form['delete']['delete'] = array( 
    '#type' => 'submit', 
    '#value' => t('Delete'),
    '#attributes' => array('class' => 'taxonomy-manager-buttons delete'), 
    '#submit' => array('taxonomy_manager_form_delete_submit'),
    '#validate' => array('taxonomy_manager_form_delete_validate'),
  );
  
  $form['delete']['cancel'] = array(
    '#type' => 'button', 
    '#attributes' => array('class' => 'taxonomy-manager-buttons cancel'),
    '#value' => t('Cancel'), 
    '#theme' => 'no_submit_button',  
  );
  
  return $form;
}

/**
 * form for adding terms
 */
function taxonomy_manager_add_form($voc, $hide_form = TRUE) {
  if ($hide_form) {
    drupal_add_js(array('hideForm' => array(
      'show_button' => 'edit-add-show', 
      'hide_button' => 'edit-add-cancel', 
      'div' => 'add-form')), 'setting');
  }
  
  $form = array();
  
  $description = "";
  if ($voc->hierarchy >= 1) {
    $description = t("If you have selected one or more terms in the tree view, the new terms are automatically children of those.");
  }
  
  $form['add'] = array( 
    '#type' => 'fieldset', 
    '#tree' => TRUE,
    '#title' => t('Add new terms'),
    '#prefix' => '<div id="add-form">',
    '#suffix' => '</div>',
    '#description' => $description,
  );
  
  for ($i=0; $i<6; $i++) {
    $form['add']['term'][$i] = array( 
      '#type' => 'textfield', 
    );
  }
  $form['add']['add'] = array( 
    '#type' => 'submit',
    '#attributes' => array('class' => 'taxonomy-manager-buttons add'), 
    '#value' => t('Add'), 
    '#validate' => array('taxonomy_manager_form_add_validate'),
    '#submit' => array('taxonomy_manager_form_add_submit'),
  );
  /*
  $form['add']['wrapper'] = array(
    '#prefix' => '<div id="more-textfields">',
    '#suffix' => '</div>',
    '#value' => ' ',
  );
  
  $form['add']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('More textfields'),
    '#description' => t("If the amount of boxes above isn't enough, click here to add more textfields."),
    '#submit' => array('poll_more_choices_submit'), // If no javascript action.
    '#ahah' => array(
      'path' => 'admin/content/taxonomy_manager/add_more',
      'wrapper' => 'more-textfields',
      'method' => 'append',
      'effect' => 'none',
    ),
  );*/
  
  $form['add']['cancel'] = array( 
    '#type' => 'button', 
    '#value' => t('Cancel'),
    '#theme' => 'no_submit_button',
    '#attributes' => array('class' => 'taxonomy-manager-buttons cancel'),  
  );

  return $form;
  
}

/*
function taxonomy_manager_add_form_more() {
  $delta = count($_POST['choice']);

  // Build our new form element.
  //$form_element = _poll_choice_form($delta);
  //drupal_alter('form', $form_element, array(), 'poll_choice_js');

  // Build the new form.
  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  // Add the new element to the stored form. Without adding the element to the
  // form, Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  $form = form_get_cache($form_build_id, $form_state);
  $form_element['name'] = array(
    '#type' => 'textfield'
  );
  $form['choice_wrapper']['choice'][$delta] = $form_element;
  form_set_cache($form_build_id, $form, $form_state);
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );

  // Rebuild the form.
  //$form = form_builder('taxonomy_manager_form', $form_element, $form_state);
  $output = drupal_render($form_element);

  drupal_json(array('status' => TRUE, 'data' => $output));
}*/

/**
 * form for merging terms
 */
function taxonomy_manager_merge_form($voc) {
  drupal_add_js(array('hideForm' => array(
    'show_button' => 'edit-merge-show', 
    'hide_button' => 'edit-merge-cancel', 
    'div' => 'merge-form')), 'setting');
  
  $form = array();
  
  $description .= t("The selected terms get merged into one term. 
    This resulting merged term can either be an exisiting term or a completely new term. 
    The selected terms will automatically get synomyms of the merged term and will be deleted afterwards.");
  
  $form['merge'] = array( 
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Merging of terms'), 
    '#description' => $description,
    '#prefix' => '<div id="merge-form">',
    '#suffix' => '</div>', 
  );
  
  $form['merge']['main_term'] = array(
    '#type' => 'textfield',
    '#title' => t('Resulting merged term'),
    '#required' => FALSE,
    '#autocomplete_path' => 'taxonomy/autocomplete/'. $voc->vid,
  );
  
  $options = array();
  
  $options['collect_parents'] = t('Collect all parents of selected terms an add it to the merged term'); 
  $options['collect_children'] = t('Collect all children of selected terms an add it to the merged term');
  $options['collect_relations'] = t('Collect all relations of selected terms an add it to the merged term');
  
  if (count($options) > 0) {
    $form['merge']['options'] = array( 
      '#type' => 'checkboxes', 
      '#title' => t('Options'), 
      '#options' => $options, 
    );
  }
   
  $form['merge']['submit'] = array( 
    '#type' => 'submit', 
    '#value' => t('Merge'),
    '#attributes' => array('class' => 'taxonomy-manager-buttons merge'), 
    '#validate' => array('taxonomy_manager_form_merge_validate'), 
    '#submit' => array('taxonomy_manager_form_merge_submit'),
  );
   
  $form['merge']['cancel'] = array( 
    '#type' => 'button', 
    '#value' => t('Cancel'),
    '#attributes' => array('class' => 'taxonomy-manager-buttons cancel'), 
    '#theme' => 'no_submit_button', 
  );
  
  return $form;
}

/**
 * form for moving terms in hierarchies
 */
function taxonomy_manager_move_form($voc) {
  drupal_add_js(array('hideForm' => array(
    'show_button' => 'edit-move-show', 
    'hide_button' => 'edit-move-cancel',
    'div' => 'move-form')), 'setting');
  
  $form = array();
  
  $description = t("You can change the parent of one or more selected terms. 
      If you leave the autocomplete field empty, the term will be a root term.");
  
  $form['move'] = array( 
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Moving of terms'),
    '#description' => $description,
    '#prefix' => '<div id="move-form">',
    '#suffix' => '</div>',
  );
  
  if ($voc->hierarchy == 2) {
    $auto_description .= t("Separate parent terms with a comma. ");
  }
  
  $form['move']['parents'] = array(
    '#type' => 'textfield',
    '#title' => t('Parent term(s)'),
    '#description' => $auto_description,
    '#required' => FALSE,
    '#autocomplete_path' => 'taxonomy/autocomplete/'. $voc->vid,
  );
  
  $options = array();
  $options['keep_old_parents'] = t('Keep old parents and add new ones (multi-parent). Otherwise old parents get replaced.');
  $form['move']['options'] = array( 
    '#type' => 'checkboxes', 
    '#title' => t('Options'), 
    '#options' => $options, 
  );
  
  $form['move']['submit'] = array( 
    '#type' => 'submit',
    '#attributes' => array('class' => 'taxonomy-manager-buttons move'), 
    '#value' => t('Move'),
    '#validate' => array('taxonomy_manager_form_move_validate'), 
    '#submit' => array('taxonomy_manager_form_move_submit'),
  );
   
  $form['move']['cancel'] = array( 
    '#type' => 'button', 
    '#value' => t('Cancel'),
    '#attributes' => array('class' => 'taxonomy-manager-buttons cancel'),
    '#theme' => 'no_submit_button',  
  );
  
  return $form;
}

/**
 * menu callback for displaying term data form
 * 
 * if this function gets called by ahah, then the term data form gets 
 * generated, rendered and return
 * otherwise, if no ahah call, redirect to original form with $vid and $tid as parameters
 *
 * @param $vid
 * @param $tid
 * @param $ahah if true, return rendered form, else redirect
 */
function taxonomy_manager_update_term_data_form($vid, $tid, $ahah = FALSE) {
  if (!$ahah) {
    drupal_goto('admin/content/taxonomy_manager/voc/'. $vid .'/'. $tid);
  }
  $form_state = array();
  $GLOBALS['devel_shutdown'] = FALSE; //prevent devel queries footprint
  
  $form = taxonomy_manager_form_term_data($tid);
  drupal_prepare_form('taxonomy_manager_form', $form, $form_state);
  $form = form_builder('taxonomy_manager_form', $form, $form_state);
  $output = drupal_render($form);
  
  print $output;
  exit();
}

/**
 * term data editing form
 *
 * @param $tid
 */
function taxonomy_manager_form_term_data($tid) {
  $term = taxonomy_get_term($tid);
  $module_path = drupal_get_path('module', 'taxonomy_manager') .'/';
  $vocabulary = taxonomy_vocabulary_load($term->vid);
  
  $form['term_data'] = array( 
    '#type' => 'fieldset', 
    '#title' => $term->name,
    '#attributes' => array('id' => 'taxonomy-term-data-fieldset'),
    '#tree' => TRUE,
  );
  
  $form['term_data']['tid'] = array(
    '#type' => 'value', 
    '#value' => $tid,
  );
  
  $form['term_data']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $term->name,
    '#size' => 35,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -20,
    '#prefix' => '<div id="term-data-name-field">',
    '#suffix' => '</div>',
  );
  
  $form['term_data']['save_name'] = array( 
    '#value' => theme("image", $module_path ."images/document-save.png", "save", NULL),
    '#prefix' => '<div id="term-data-name-save">',
    '#suffix' => '</div><div class="clear"></div>', 
  );
  
  $form['term_data']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $term->description,
    '#cols' => 35,
    '#rows' => 4,
    '#prefix' => '<div id="term-data-description"><div id="term-data-description-field">',
    '#suffix' => '</div>',
  );
  
  $form['term_data']['save_description'] = array( 
    '#value' => theme("image", $module_path ."images/document-save.png", "save", NULL),
    '#prefix' => '<div id="term-data-description-save">',
    '#suffix' => '</div></div><div class="clear"></div>', 
  );
  
  $form['term_data']['synonyms'] = _taxonomy_manager_form_term_data_lists($term, taxonomy_get_synonyms($term->tid), t('Synonyms'), 'synonym', FALSE); 
  $form['term_data']['synonyms']['#tree'] = TRUE;
  
  $form['term_data']['relations'] = _taxonomy_manager_form_term_data_lists($term, taxonomy_get_related($term->tid), t('Relations'), 'related'); 
  $form['term_data']['relations']['#tree'] = TRUE;
  
  $parents = taxonomy_get_parents($term->tid);
  $p_add = TRUE;
  $form['term_data']['parents'] = _taxonomy_manager_form_term_data_lists($term, $parents, t('Parents'), 'parent', TRUE); 
  $form['term_data']['parents']['#tree'] = TRUE;
  
  $form['term_data']['weight'] = array( 
    '#type' => 'weight', 
    '#default_value' => $term->weight, 
    '#delta' => 40,
    '#prefix' => '<div id="term-data-weight">', 
    '#suffix' => '</div>', 
    '#title' => t('Weight'), 
  );
  return $form;
}

/**
 * helper function for generating tables with values and delete op and field for adding
 *
 * @param $term term object which is going to be displayed
 * @param $values array of values to show, e.g related terms, synonyms, parents, children
 * @param $header_type string to display as header
 * @param $attr attribute type to show, can be 'related', 'synonym', 'parent', 'child'
 * @param $autocomplete if true, adds autocomplete, else a textfield
 * @param $add if true, shows add operation
 * @return an form array
 */
function _taxonomy_manager_form_term_data_lists($term, $values, $header_type, $attr, $autocomplete = TRUE, $add = TRUE) {
  $module_path = drupal_get_path('module', 'taxonomy_manager') .'/';
  $rows = array();
  
  foreach ($values as $tid => $value) {
    $row = array();
    if (is_object($value)) {
      $name = $value->name;
      $id = $value->tid;
    }
    else {
      $name = $value;
      $id = $value;
    }
    $row[] = array(
      'data' => $name, 
      'class' => 'taxonomy-term-data-name',
      'id' => 'term-'. $id, 
    );
    $row[] = array(
      'data' => theme("image", $module_path ."images/list-remove.png", "term-remove", NULL, array('class' => $attr)), 
      'class' => 'taxonomy-term-data-operations', 
    );
    $rows[] = $row;
  }
  
  $headers = array(t('!type', array('!type' => $header_type)), '');
  
  
  $form['list'] = array( 
    '#value' => theme('table', $headers, $rows, array('class' => 'taxonomy-term-data-table')),
  );
  if ($add) {
    $form['add'] = array( 
      '#type' => 'textfield',
      '#prefix' => '<div class="term-data-autocomplete-line"><div class="term-data-autocomplete">',
      '#suffix' => '</div>',
      '#size' => 35,
    );
    if ($autocomplete) {
      $form['add']['#autocomplete_path'] = 'taxonomy/autocomplete/'. $term->vid;
    }
  
    $form['add_button'] = array( 
      '#value' => theme("image", $module_path ."images/list-add.png", "add", NULL, array('class' => $attr)), 
      '#prefix' => '<div class="term-data-autocomplete-add">',
      '#suffix' => '</div><div class="clear"></div></div>',
    );
  }
  
  return $form;

}

/**
 * validates the form (only search button)
 **/
function taxonomy_manager_form_validate($form, &$form_state) {
  if (empty($form_state['values']['search']['field'])) {
    form_set_error('search', t('Search field is empty'));
  }
}

/**
 * submits the taxonomy manager form (only search button)
 **/
function taxonomy_manager_form_submit($form, &$form_state) {  
  $search_string = $form_state['values']['search']['field'];
  $terms = array();
  $terms = taxonomy_manager_autocomplete_tags_get_tids($search_string, $form_state['values']['vid'], FALSE);
  $term = array_shift($terms);
  $tid = $term['tid'];
  if ($tid) {
    drupal_goto('admin/content/taxonomy_manager/voc/'. $form_state['values']['vid'] .'/'. $tid);
  }
  else {
    drupal_goto('admin/content/taxonomy_manager/voc/'. $form_state['values']['vid'] .'/0/'. $search_string);
  }
}


/**
 * Validation handler for adding terms
 *
function taxonomy_manager_form_add_validate($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  //check for parents concerning voc settings
  $voc = taxonomy_vocabulary_load($form_state['values']['vid']);
      
  if ($voc->hierarchy == 0 && count($selected_tids) > 0) {
    form_set_error('add', t('Please unselect terms in the list before adding new terms. This vocabulary doesn\'t provide hierarchies.'));
  }
  else if ($voc->hierarchy == 1 && count($selected_tids) > 1) {
    form_set_error('add', t('Please only select one term in the list (at maximum) as parent. This vocabulary provides only single hirarchies.'));
  }
  
}*/

/**
 * Submit handler for adding terms
 */
function taxonomy_manager_form_add_submit($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  foreach ($form_state['values']['add']['term'] as $value) {
    if (!empty($value)) {
      $term = array();
      $term['name'] = $value;
      $term['vid'] = $form_state['values']['vid'];
      $term['parent'] = $selected_tids;
      taxonomy_save_term($term);
    }
  }
  drupal_set_message("Terms added");
}


/**
 * Validation handler for deleting terms
 */
function taxonomy_manager_form_delete_validate($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  
  if (count($selected_tids) < 1) {
    form_set_error('delete', t("No terms for deleting selected"));
  }
}

/**
 * Submit handler for deleting terms
 */
function taxonomy_manager_form_delete_submit($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  taxonomy_manager_delete_terms($selected_tids, $form_state['values']['delete']['options']);
  drupal_set_message("Selected terms deleted"); 
}

/**
 * Validation handler for moving terms
 */
function taxonomy_manager_form_move_validate($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  if (count($selected_tids) < 1) {
    form_set_error('move', t("Please selected terms you want to move in the hierarchy"));
  }
}

/**
 * Submit handler for moving terms
 */
function taxonomy_manager_form_move_submit($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  $typed_parents = taxonomy_manager_autocomplete_tags_get_tids($form_state['values']['move']['parents'], $form_state['values']['vid']);
  
  $parents = array();
  foreach ($typed_parents as $parent_info) {
    $parents[] = $parent_info['tid'];
  }
  if (count($parents) == 0) $parents[0] = 0; //if empty, delete all parents
  taxonomy_manager_move($parents, $selected_tids, $form_state['values']['move']['options']);
      
  drupal_set_message("Terms moved");
}

/**
 * Validation handler for validating terms
 */
function taxonomy_manager_form_merge_validate($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  
  $main_terms = array();
  $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
  preg_match_all($regexp, $form_state['values']['merge']['main_term'], $matches);
  $main_terms = $matches[1];

  if (!is_array($main_terms) || count($main_terms) == 0 || empty ($main_terms[0])) {
    form_set_error('merge][main_term', t("Please enter a name into %title", array('%title' => "'". t('Main term') ."'")));
  }
  else if (count($main_terms) > 1) {
    form_set_error('merge][main_term', t("Please only enter single names into %title", array('%title' => "'". t('Main term') ."'")));
  }
      
  if (count($selected_tids) < 1) {
    form_set_error('merge', t("Please selected terms you want to merge"));
  }
  else if (count($selected_tids) > 50) {
    form_set_error('merge', t("Please select less than 50 terms to merge. Merging to many terms in one step can cause timeouts and inconsistent database states"));
  }
}

/**
 * Submit handler for merging terms
 */
function taxonomy_manager_form_merge_submit($form, $form_state) {
  $selected_tids = array();
  $selected_tids = $form_state['values']['taxonomy']['manager']['tree']['selected_terms'];
  $main_terms = taxonomy_manager_autocomplete_tags_get_tids($form_state['values']['merge']['main_term'], $form_state['values']['vid']);
  $main_term = array_shift($main_terms);
  
  $new_inserted = false;
  if ($main_term['new']) $new_inserted = true;
  $main_term_tid = $main_term['tid'];
  taxonomy_manager_merge($main_term_tid, $selected_tids, $form_state['values']['merge']['options'], $new_inserted);
  drupal_set_message("Terms merged");
}

/**
 * Defines a settings form.
 */
function taxonomy_manager_settings() {
  $form['taxonomy_manager_disable_mouseover_weights'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable mouse-over effect for weight arrows'),
    '#default_value' => variable_get('taxonomy_manager_disable_mouseover_weights', 0),
    '#description' => t('Disabeling this feature speeds up the Taxonomy Manager'),
  );
  return system_settings_form($form);
}

/**
 * callback handler for updating term data
 *
 * @param $vid
 */
function taxonomy_manager_term_data_edit($vid) {
  $param = $_POST;

  $tid = $param['tid'];
  $values = $param['value'];
  $attr_type = $param['attr_type'];
  $op = $param['op'];
  
  $term = taxonomy_get_term($tid);
  
  if ($term->vid != $vid) {
    return;
  }
  
  if ($op == 'add') {
    if ($attr_type == 'synonym') {
      $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
      preg_match_all($regexp, $values, $matches);
      $synonyms = array_unique($matches[1]);
      $values = array();
      foreach ($synonyms as $syn) {
        $values[] = trim($syn);
      }
    }
    else {
      $typed_term_tids = array();
      $typed_term_tids = taxonomy_manager_autocomplete_tags_get_tids($values, $term->vid);
      $values = array();
      foreach ($typed_term_tids as $term_info) {
        $values[] = $term_info['tid'];
      }
    }
  }
  
  switch ($attr_type) {
    case 'name': 
      db_query("UPDATE {term_data} SET name = '%s' WHERE tid = %d", $values, $tid);
      break;
      
    case 'description': 
      db_query("UPDATE {term_data} SET description = '%s' WHERE tid = %d", $values, $tid);
      break;
    
    case 'parent':
      if (!is_array($values)) $values = array($values);
      foreach ($values as $value) {
        db_query("DELETE FROM {term_hierarchy} WHERE parent = %d AND tid = %d", $value, $tid);
        if ($op == 'add') {
          db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES (%d, %d)", $value, $tid);
        }
      }
      if ($op == 'delete') {
        $parents = taxonomy_get_parents($tid);
        if (count($parents) == 0) {
          //ensure that a term has a least parent 0
          db_query("DELETE FROM {term_hierarchy} WHERE parent = 0 AND tid = %d", $tid);
          db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES (0, %d)", $tid);
        }
      }
      else if ($op == 'add') {
        db_query("DELETE FROM {term_hierarchy} WHERE parent = 0 AND tid = %d", $tid);
      }
      break; 
    
    case 'related': 
      if (!is_array($values)) $values = array($values);
      foreach ($values as $value) {
        if ($value != 0) {
          db_query("DELETE FROM {term_relation} WHERE tid1 = %d AND tid2 = %d", $tid, $value);
        //  db_query("DELETE FROM {term_relation} WHERE tid2 = %d AND tid1 = %d", $tid, $value);
          if ($op == 'add') {
            db_query('INSERT INTO {term_relation} (tid1, tid2) VALUES (%d, %d)', $tid, $value);
          }
        }
      }
      break;
    
    case 'synonym': 
      if (!is_array($values)) $values = array($values);
      foreach ($values as $value) { 
        db_query("DELETE FROM {term_synonym} WHERE tid = %d AND name = '%s'", $tid, $value);
        if ($op == 'add') {
          db_query("INSERT INTO {term_synonym} (tid, name) VALUES (%d, '%s')", $tid, $value);  
        }
      }
      break;
    
    case 'weight': 
      if (is_numeric($values)) {
        db_query("UPDATE {term_data} SET weight = %d WHERE tid = %d", $values, $tid);
      }
      break;
  }

  module_invoke_all('taxonomy', 'update', 'term', (array) taxonomy_get_term($tid));
  module_invoke_all('taxonomy_manager_term_data_submit', $param, $values);
  exit();
}
/**
 * checks if voc has terms
 *
 * @param $vid voc id
 * @return true, if terms already exists, else false
 */
function _taxonomy_manager_voc_is_empty($vid) {
  $count = db_result(db_query_range("SELECT t.* FROM {term_data} t INNER JOIN {term_hierarchy} h ON t.tid = h.tid WHERE vid = %d AND h.parent = 0", $vid, 0, 1));
  if ($count == 0) return true;
  return false;
}


/**
 * deletes terms from the database
 * optional orphans (terms where parent get deleted) can be deleted as well
 * 
 * (difference to taxonomy_del_term: deletion of orphans optional)
 *
 * @param $tids array of term id to delete
 * @param $options associative array with options
 *   if $options['delete_orphans'] is true, orphans get deleted
 */
function taxonomy_manager_delete_terms($tids, $options = array()) {
  if (!is_array($tids)) array($tids);
  while (count($tids) > 0) {
    $orphans = array();
    foreach ($tids as $tid) {      
      if ($children = taxonomy_get_children($tid)) {
        foreach ($children as $child) {
          $parents = taxonomy_get_parents($child->tid);
          if ($options['delete_orphans']) {
            if (count($parents) == 1) {
              $orphans[] = $child->tid;
            }
          }
          else {
            db_query("DELETE FROM {term_hierarchy} WHERE tid = %d AND parent = %d", $child->tid, $tid);
            if (count($parents) == 1) {
              if (!db_result(db_query("SELECT COUNT(*) FROM {term_hierarchy} WHERE tid = %d AND parent = 0", $child->tid))) {
                db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES(0, %d)", $child->tid);          
              }
            }
          }
        }
      }
      
      db_query('DELETE FROM {term_data} WHERE tid = %d', $tid);
      db_query('DELETE FROM {term_hierarchy} WHERE tid = %d', $tid);
      db_query('DELETE FROM {term_relation} WHERE tid1 = %d OR tid2 = %d', $tid, $tid);
      db_query('DELETE FROM {term_synonym} WHERE tid = %d', $tid);
      db_query('DELETE FROM {term_node} WHERE tid = %d', $tid);
    
      $term = (array) taxonomy_get_term($tid);
      module_invoke_all('taxonomy', 'delete', 'term', $term);
      $tids = $orphans;
    }
  } 
}


/**
 * moves terms in hierarchies to other parents
 *
 * @param $parents 
 *   array of parent term ids to where children can be moved
 *   array should only contain more parents if multi hiearchy enabled 
 *   if array contains 0, terms get placed to first (root) level
 * @param $children
 *   array of term ids to move
 * @param $options
 *   array of additional options for moving
 *   'keep_old_parents': if true, exisiting parents doesn't get deleted (only possible with multi hierarchies)
 */
function taxonomy_manager_move($parents, $children, $options = array()) {
  if (!is_array($parents)) array($parents);
  
  foreach ($children as $child) {
    if (!$options['keep_old_parents']) {
      db_query("DELETE FROM {term_hierarchy} WHERE tid = %d", $child);
    }
    foreach ($parents as $parent) {
      db_query("INSERT INTO {term_hierarchy} (parent, tid) VALUES (%d, %d)", $parent, $child);
    }
  }
}

/**
 * merges terms into another term (main term), all merged term get added
 * to the main term as synonyms. 
 * term_node relations are updated automatically (node with one of merging terms gets main term assigned)
 * after all opterions are done (adding of hierarchies, relations is optional) merging
 * terms get deleted
 *
 * @param $main_term
 *   id of term where other terms get merged into
 * @param $merging_terms
 *   array of term ids, which get merged into main term and afterwards deleted
 * @param $options
 *   array with additional options, possible values:
 *   'collect_parents': if true, all parents of merging terms get added to main term (only possible with multi hierarchies)
 *   'collect_children': if true, all children of merging terms get added to main term
 *   'collect_relations': if true, all relations of merging terms are transfered to main term
 */
function taxonomy_manager_merge($main_term, $merging_terms, $options = array(), $new_inserted = TRUE) {
  $vid = db_result(db_query("SELECT vid FROM {term_data} WHERE tid = %d", $main_term));
  $voc = taxonomy_vocabulary_load($vid);
  $merging_terms_parents = array();
  
  if ($voc->hierarchy == 2 && $new_inserted && $options['collect_parents']) {
    db_query("DELETE FROM {term_hierarchy} WHERE parent = 0 AND tid = %d", $main_term);
  }
  
  //TODO: add hook, so that other modules can consider changes
  foreach ($merging_terms as $merge_term) {
    if ($merge_term != $main_term) {
      //update node-relations
      $sql = db_query("SELECT nid FROM {term_node} WHERE tid = %d", $merge_term);
      while ($obj = db_fetch_object($sql)) {
        $nid = $obj->nid;
        db_query("DELETE FROM {term_node} WHERE tid = %d AND nid = %d", $merge_term, $nid);
        if (!db_result(db_query("SELECT COUNT(*) FROM {term_node} WHERE tid = %d AND nid = %d", $main_term, $nid))) {
          db_query("INSERT INTO {term_node} (tid, nid) VALUES (%d, %d)", $main_term, $nid);
       }
      }
    
      if ($voc->hierarchy == 1) {  //sinlge hierarchy
        $parents = taxonomy_get_parents($merge_term);
        $parent = array_shift($parents);
        $merging_terms_parents[$parent->tid] = $parent->tid;
      }
      if ($options['collect_parents']) {
        $parents = taxonomy_get_parents($merge_term);
        foreach ($parents as $parent_tid => $parent_term) {
          if (!db_result(db_query("SELECT COUNT(*) FROM {term_hierarchy} WHERE tid = %d AND parent = %d", $main_term, $parent_tid))) {
             db_query("INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d, %d)", $main_term, $parent_tid);
          }
        }
      }
    
      if ($options['collect_children']) {
        $children = taxonomy_get_children($merge_term);
        foreach ($children as $child_tid => $child_term) {
          if (!db_result(db_query("SELECT COUNT(*) FROM {term_hierarchy} WHERE tid = %d AND parent = %d", $child_tid, $main_term))) {
            db_query("INSERT INTO {term_hierarchy} (tid, parent) VALUES (%d, %d)", $child_tid, $main_term);
          }
        }
      }
    
      if ($options['collect_relations']) {
        $relations = taxonomy_get_related($merge_term);
        foreach ($relations as $related_tid => $relation) {
          if ($relation->tid1 == $merge_term) {
            if (!db_result(db_query("SELECT COUNT(*) FROM {term_relation} WHERE tid1 = %d AND tid2 = %d", $main_term, $related_tid))) {
              db_query("INSERT INTO {term_relation} (tid1, tid2) VALUES (%d, %d)", $main_term, $related_tid);
            }
          }
          else if ($relation->tid2 == $merge_term) {
            if (!db_result(db_query("SELECT COUNT(*) FROM {term_relation} WHERE tid2 = %d AND tid1 = %d", $main_term, $related_tid))) {
              db_query("INSERT INTO {term_relation} (tid2, tid1) VALUES (%d, %d)", $main_term, $related_tid);
            }
          }
        }
      }
    
      //save merged term (and synonomys of merged term) as synonym
      $term = taxonomy_get_term($merge_term);
      $merge_term_synonyms = taxonomy_get_synonyms($merge_term);
      $merge_term_synonyms[] = $term->name;
      foreach ($merge_term_synonyms as $syn) {
        if (!db_result(db_query("SELECT COUNT(*) FROM {term_synonym} WHERE tid = %d AND name = '%s'", $main_term, $syn))) {
          db_query("INSERT INTO {term_synonym} (tid, name) VALUES (%d, '%s')", $main_term, $syn);
        }
      }
    
      taxonomy_manager_delete_terms(array($merge_term));
    }
  }

  if ($voc->hierarchy == 1 && count($merging_terms_parents) == 1 && $new_inserted) {
    db_query("UPDATE {term_hierarchy} SET parent = %d WHERE tid = %d", array_shift($merging_terms_parents), $main_term);
  }
  
  taxonomy_manager_merge_history_update($main_term, $merging_terms);
}

/**
 * inserts merging information (main_tid - merged_tid ) into taxonomy_manager_merge
 * and updates cache, which is used to reconstructs taxonomy/term pages
 *
 * @param $main_tid term if of main term
 * @param $merged_tids array of merged term ids
 */
function taxonomy_manager_merge_history_update($main_tid, $merged_tids) {
  if (!is_array($merged_tids)) (array) $merged_tids;
  
  foreach ($merged_tids as $merged_tid) {
    if ($merged_tid != $main_tid) {
      //check if merged term has been a main term once before
      $check_merged = db_result(db_query("SELECT COUNT(*) FROM {taxonomy_manager_merge} WHERE main_tid = %d", $merged_tid));
  
      if ($check_merged) {
        db_query("UPDATE {taxonomy_manager_merge} SET main_tid = %d WHERE main_tid = %d", $main_tid, $merged_tid);
      }
      //insert into merging history
      db_query("INSERT INTO {taxonomy_manager_merge} (main_tid, merged_tid) VALUES (%d, %d)", $main_tid, $merged_tid);
    }
  }
  taxonomy_manager_merge_history_update_cache();
}


/**
 * helper function for getting out of term ids from autocomplete fields
 * non-exsiting terms get inserted autmatically
 * (part of taxonomy_node_save)
 *
 * @param $typed_input input string of form field
 * @param $vid vocabulary id
 * @return array of term ids
 */
function taxonomy_manager_autocomplete_tags_get_tids($typed_input, $vid, $insert_new = TRUE) {
  $tids = array();
  
  $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
  preg_match_all($regexp, $typed_input, $matches);
  $typed_terms = array_unique($matches[1]);

  foreach ($typed_terms as $typed_term) {
    $typed_term = str_replace('""', '"', preg_replace('/^"(.*)"$/', '\1', $typed_term));
    $typed_term = trim($typed_term);
    if ($typed_term == "") { continue; }

    $possibilities = taxonomy_get_term_by_name($typed_term);
    $typed_term_tid = NULL; // tid match if any.
    foreach ($possibilities as $possibility) {
      if ($possibility->vid == $vid) {
        $typed_term_tid = $possibility->tid;
        $tids[$typed_term_tid]['tid'] = $typed_term_tid;
      }
    }

    if (!$typed_term_tid && $insert_new) {
      $edit = array('vid' => $vid, 'name' => $typed_term);
      $status = taxonomy_save_term($edit);
      $typed_term_tid = $edit['tid'];
      $tids[$typed_term_tid]['tid'] = $typed_term_tid;
      $tids[$typed_term_tid]['new'] = TRUE;
    }
  }
  return $tids;
}

/**
 * callback for updating weights
 * data send through AJAX, $_POST
 * $_POST[$tid] => $weight
 *
 */
function taxonomy_manager_update_weights($vid) {
  $weights = $_POST;
  if (is_array($weights)) {
    foreach ($weights as $tid => $weight) {
      if (is_numeric($tid) && is_numeric($weight)) {
        if (_taxonomy_manager_tree_term_valid($tid, $vid)) {
          db_query("UPDATE {term_data} SET weight = %d WHERE tid = %d", $weight, $tid);
        }
      }
    }
  }
  exit();
}


/**
 * theme function for taxonomy manager form
 */
function theme_taxonomy_manager_form($form) {
  $pager = theme('pager', NULL, TAXONOMY_MANAGER_TREE_PAGE_SIZE, 0);
  $tree = drupal_render($form['taxonomy']);
  $term_data = drupal_render($form['term_data']);
  $top = drupal_render($form);
  $output = $top . $pager;
  $output .= '<div id="taxonomy-manager">';
  $output .= $tree;
  $output .= '<div id="taxonomy-term-data">';
  $output .= $term_data;
  $output .= '</div>';
  $output .= '<div class="clear" />';
  $output .= '</div>';
  
  return $output;
}

/**
 * themes a real button form type (no form submit)
 */
function theme_no_submit_button($element) {
  // Make sure not to overwrite classes.
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'] .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'];
  }

  return '<input type="button" '. (empty($element['#name']) ? '' : 'name="'. $element['#name'] .'" ')  .'id="'. $element['#id'] .'" value="'. check_plain($element['#value']) .'" '. drupal_attributes($element['#attributes']) ." />\n";
}

/**
 * themes a image type button
 */
function theme_taxonomy_manager_image_button($element) {

  //Make sure not to overwrite classes
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'] .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'];
  }

  // here the novelty begins: check if #button_type is normal submit button or image button
  $return_string = '<input ';
  if ($element['#button_type'] == 'image') {
    $return_string .= 'type="image" ';
  }
  else {
    $return_string .= 'type="submit" ';
  }
  $return_string .= (empty($element['#id']) ? '' : 'id="'. $element['#id'] .'" ');
  $return_string .= (empty($element['#name']) ? '' : 'name="'. $element['#name'] .'" ');
  $return_string .= 'value="'. check_plain($element['#value']) .'" ';
  $return_string .= drupal_attributes($element['#attributes']) ." />\n";
 
  return $return_string;

}
