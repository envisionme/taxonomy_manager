// $Id$

README - TAXONOMY MANAGER 
**************************


SHORT PROJECT DESCRIPTION
--------------------------
This module provides an additional powerful for managing vocabularies of the taxonomy module.
It's especially for long sets of vocabularies very useful.

Features:
  * dynamic tree view
  * deleting of selected terms
  * mass adding of new terms
  * moving of terms in hierarchies
  * merging of terms
  * fast weight changing with up and down arrows (and AJAX saving)
  * AJAX powered term editing form
  * simple search interface




REQUIREMENTS
------------
  - Taxonomy module enabled
  - JavaScript enabled in your browser
  - a user with 'administer taxonomy' permission


INSTALLATION
------------
1. Place the entire taxonomy_manager directory into your Drupal sites/all/modules/ directory.

2. Enable the taxonomy manager module by navigating to:

     administer > site building > modules


USING THE TAXONOMY MANAGER
--------------------------
To use the Taxonomy Manager go to administer > content management > taxonomy manager. This page
contains a list of all available vocabularies. By clicking at one of the vocabularies, you get 
redirected to the Taxonomy Manager interface, where you can edit the whole tree structure and
terms. 
If you want to edit any general vocabulary settings or if you want to create a new one, go to 
the categories (administer > content management > categories) page.


 - Tree View
     The tree view shows all terms of the vocabulary with their hierarchical relations. If your
     term list gets very long, there is a paging mechanism included with a page size of 50 terms. 
     If you are having hierarchical vocabularies, all parent terms get have a plus sign, which 
     means you can expand them to show their children terms. If any children terms has again some
     children terms assigned, you can expanded them (and so on). Use the minus symbol to collapse
     them again.
     In multiple hierarchies, if you are having terms with more parents, the term gets shown under 
     each of its parents. 
     
 - Adding of terms
     For adding new term, click on the 'Add' Button. A fieldset containing some textfields expands.
     If you want to close this fieldset, click 'Cancel'.
     To insert a new term, fill in any textfield. Each textfield can only contain one term. 
     You don't have to fill in all textfields, they can be left empty. 
     Depending on your hierarchy settings, it's possible to insert terms and to directly assign 
     a parent to them (they get child terms). If you want to do this, select a parent in the 
     tree structure by marking the checkbox. If you have multiple hierarchies enabled, it's even
     possible to assign the new inserted terms to more parents at once by selecting more terms 
     in the tree structure.
     
 - Weight Editing
     Every term has a weight. This weight determines the position the terms get listed. If terms
     have the same weight, they are ordered alphabetically. 
     If you want to change the weight, you have 3 ways to do that.
       1st way: select the terms you want to move one place up (can be more at once) and press
                either the up or the down button in the toolbar. Every selected terms gets
                moved exactly one position. All saving is done automatically through AJAX.
       2nd way: every term in tree view has a mouseover effect. When you hover over a term, two
                small up and down arrows will appear. Click them to move this term by one
                position.
       3rd way: click on the term, where you want to change the weight. A form for editing the 
                term data appears on the right side of the tree view. At the bottom of this 
                form, there is a select field, which shows the current weight. By changing the
                value, the tree view gets automatically reordered and the values are saved to the
                database through AJAX.
 
 - Deleting
     If you want to delete terms from the vocabulary, select them by marking the checkbox and click
     the 'Delete' button. A fieldset, where you have to confirm the deletion, expands. 
     For hierarchical vocabularies (single or multi), the fieldset contains a option, which says:
     'Delete children of selected, if there are any'. Check this if you want to delete all children 
     of a selected parent term. Otherwise, if you are deleting the last parent of terms, the terms
     get added to root level.

 - Moving
     This operation is only available in hierarchical (single or multiple) vocabularies. It allows
     you to change hierarchies by moving terms from one parent to one other. Of course it's possible
     to move terms from the root level (they have no parent) under a parent term and the other way round.
     Select all terms you want to move by marking the checkbox. Click the 'Move' button. A fieldset with
     some options expands.
     This fielset contains a autocomplete field, where you have to determine the parent term (under which
     the terms should be moved). If you want to move terms to the root level, leave this field empty. 
     This autocomplete form allows you to either choose a parent term from the list of exisitng terms
     or to insert a new terms, which will be used as parent (this parent term will be added to the root 
     level). 
     In multiple hierarchical vocabularies, it's possible to move terms to more parents in one step by
     inserting more terms into the autocomplete field and separating them by commas. Additional, there
     appears an option ('Keep old parents and add new one'), which prevents the replacing of old parents.
 
 - Merging
     With the merging action, you can put terms with the same meaning together (e.g. your vocabulary
     contains: SoC, Summer of Code, GSoC, Google Summer of Code). All terms, that get merged into 
     one other, get synonyms of the term, they are merged into (here called the main term). Additional
     all term-node association gets automatically updated (this means nodes, that had a merged term
     assigned, now get the main term instead). All merged terms are deleted afterwards. 
     In the Taxonomy Manager, you can do that by selecting all terms you want to merge and to click
     the 'Merge' button. A fieldset with an autocomplete field an some options expands. In the 
     autocomplete field you have to specify the main term (into which the selected get merged). The
     main term can be either chosen from the list of existing terms or can be inserted automatically
     and used as main term.
     Additional, there are some options available (they depend on the vocabulary settings). If you want
     to add any kind of relations (parents, children, related terms) from the merged terms to the
     main term, select one (or more) of them.
     
     NOTE: At the moment, the Taxonomy Manager only cares about the term-node association inserted
           into the term_node table (by the taxonomy module). If you are using any CCK modules, like 
           CCK Taxonomy or Content Taxonomy, which (can) save the term - node association in cck tables, 
           don't use the Merging action, because changes are not handled.
           If you are using Views filters instead of the default taxonomy term page, merged terms are 
           either respected.
           If you want to customize this by yourself or have some other module, you can use following 
           functiontaxonomy_manager_merge_get_main_term($tid) for getting the main term id (if there 
           is any main term, else return 0). The term merge history gets saved in the 
           taxonomy_manager_merge table (main_tid, merged_tid) and gets additional cached, so that 
           checking for a merged terms causes nearly no performance loss.
 
 - Editing term data
     If you want to edit or read some term properties, click on the term. A fieldset on the right side
     of the tree view gets loaded. This contains all term related information and can be edited. If you
     want to change the term name or the description, fill in any changes you want and click the saving 
     symbol. All saving is done through AJAX, so no reload is necessary.
     Additional, this page contains listing of synonyms, related terms and parents (depends of course on
     your vocabulary settings). 
     Every listed entry has an delete operation. By clicking the delete symbol, the relation gets deleted.
     In case of synonyms, the names get deleted from the database. If you are deleting a related term or a 
     parent, this doesn't delete the term itself, only the relation. 
     For adding new synonyms, the listing has a textfield below. Insert there any new synonym and click the 
     plus symbol.
     For adding a new related term or a new parent (if multi hierarchy), there is a autocomplete field below
     the listing. Use this inserted new terms and use them as related term / parent or assign existing
     terms. 
 
 - Using the search
     At the top of the page, there is a collapsed fieldset, called 'Search'. This search allows you to 
     directly select an existing term for editing. Else, if your input doesn't contain an existing term, 
     the value will be used for filtering root level terms (but this doesn't affect any child term).


AUTHOR
------
Matthias Hutterer 
User: mh86@drupal.org
Email: m_hutterer@hotmail.com
