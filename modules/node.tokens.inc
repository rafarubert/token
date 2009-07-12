<?php
// $Id$

/**
 * @file
 * Builds placeholder replacement tokens for node-related data.
 */

/**
 * Implement hook_tokens().
 */
function node_tokens($type, $tokens, $data, $language = NULL, $sanitize = TRUE) {
  $url_options = array('absolute' => TRUE);
  if (isset($language)) {
    $url_options['language'] = $language;
  }

  $replacements = array();

  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Simple key values on the node.
        case 'nid':
          $replacements[$original] = $node->nid;
          break;

        case 'vid':
          $replacements[$original] = $node->vid;
          break;

        case 'tnid':
          $replacements[$original] = $node->tnid;
          break;

        case 'uid':
          $replacements[$original] = $node->uid;
          break;

        case 'name':
          $replacements[$original] = $sanitize ? filter_xss($node->name) : $node->name;
          break;

        case 'title':
          $replacements[$original] = $sanitize ? filter_xss($node->title) : $node->title;
          break;

        case 'body':
          if (!empty($node->body)) {
            $replacements[$original] = $sanitize ? $node->body[0]['safe'] : $node->body[0]['value'];
          }
          break;

        case 'summary':
          if (!empty($node->body)) {
            $replacements[$original] = $sanitize ? $node->body[0]['safe_summary'] : $node->body[0]['summary'];
          }
          break;

        case 'type':
          $replacements[$original] = $sanitize ? check_plain($node->type) : $node->type;
          break;

        case 'type-name':
          $type_name = node_get_types('name', $node->type);
          $replacements[$original] = $sanitize ? check_plain($type_name) : $type_name;
          break;

        case 'language':
          $replacements[$original] = $sanitize ? check_plain($node->language) : $node->language;
          break;

        case 'url':
          $replacements[$original] = url('node/' . $node->nid, array('absolute' => TRUE));
          break;

        case 'edit-url':
          $replacements[$original] = url('node/' . $node->nid . '/edit', array('absolute' => TRUE));
          break;

        // Default values for the chained tokens handled below.
        case 'author':
          $replacements[$original] = $sanitize ? filter_xss($node->name) : $node->name;
          break;

        case 'created':
          $replacements[$original] = format_date($node->created, 'medium', '', NULL, (isset($language) ? $language->language : NULL));
          break;

        case 'changed':
          $replacements[$original] = format_date($node->changed, 'medium', '', NULL, (isset($language) ? $language->language : NULL));
          break;
      }
    }

    if ($author_tokens = token_find_with_prefix($tokens, 'author')) {
      $author = user_load($node->uid);
      $replacements += module_invoke_all('tokens', 'user', $author_tokens, array('user' => $author), $language, $sanitize);
    }

    if ($created_tokens = token_find_with_prefix($tokens, 'created')) {
      $replacements += module_invoke_all('tokens', 'date', $created_tokens, array('date' => $node->created), $language, $sanitize);
    }

    if ($changed_tokens = token_find_with_prefix($tokens, 'changed')) {
      $replacements += module_invoke_all('tokens', 'date', $changed_tokens, array('date' => $node->changed), $language, $sanitize);
    }
  }

  return $replacements;
}

/**
 * Implement hook_token_info().
 */
function node_token_info() {
  $results = array();

  $type['name'] = t('Nodes');
  $type['description'] = t('Tokens related to individual nodes.');


  // Core tokens for nodes.
  $node['nid'] = array(
    'name' => t("Node ID"),
    'description' => t("The unique ID of the node."),
  );

  $node['vid'] = array(
    'name' => t("Revision ID"),
    'description' => t("The unique ID of the node's latest revision."),
  );

  $node['tnid'] = array(
    'name' => t("Translation set ID"),
    'description' => t("The unique ID of the original-language version of this node, if one exists."),
  );

  $node['uid'] = array(
    'name' => t("User ID"),
    'description' => t("The unique ID of the user who posted the node."),
  );

  $node['type'] = array(
    'name' => t("Content type"),
    'description' => t("The type of the node."),
  );

  $node['type-name'] = array(
    'name' => t("Content type name"),
    'description' => t("The human-readable name of the node type."),
  );

  $node['title'] = array(
    'name' => t("Title"),
    'description' => t("The title of the node."),
  );

  $node['body'] = array(
    'name' => t("Body"),
    'description' => t("The main body text of the node."),
  );

  $node['summary'] = array(
    'name' => t("Summary"),
    'description' => t("The summary of the node's main body text."),
  );

  $node['language'] = array(
    'name' => t("Language"),
    'description' => t("The language the node is written in."),
  );

  $node['url'] = array(
    'name' => t("URL"),
    'description' => t("The URL of the node."),
  );

  $node['edit-url'] = array(
    'name' => t("Edit URL"),
    'description' => t("The URL of the node's edit page."),
  );


  // Chained tokens for nodes.
  $node['created'] = array(
    'name' => t("Date created"),
    'description' => t("The date the node was posted."),
    'references' => 'date',
  );

  $node['changed'] = array(
    'name' => t("Date changed"),
    'description' => t("The date the node was most recently updated."),
    'references' => 'date',
  );

  $node['author'] = array(
    'name' => t("Author"),
    'description' => t("The author of the node."),
    'references' => 'user',
  );

  $results['types']['node'] = $type;
  $results['tokens']['node'] = $node;
  return $results;
}