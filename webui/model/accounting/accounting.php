<?php

class ModelAccountingAccounting extends Model {

   public function run_counters($start=NULL, $stop = NULL, $column = 'arrived') {
      $now = time();
      $counter = array();
      $data = array(
                      'starttimestamp' => 0,
                      'stoptimestamp' => 0,
                      'addedstats' => 0,
                      'deletedstats' => 0
                     );

      if(!in_array($column, array('sent', 'arrived'))) { return $data; }

      if(!is_null($start)) {
         $start = $this->__decodeDate($start);
      } else {
         //if we got nothing, then operate on today
         $start = $this->__decodeDate("00:00:00");
      }

      if(!is_null($stop)) {
         $stop = $this->__decodeDate($stop);
      } else {
         //if we got nothing, then operate on today
         $stop = $this->__decodeDate("00:00:00");
      }
      $stop += 86400;


      $data['starttimestamp'] = $start;
      $data['stoptimestamp'] = $stop;


      // emails sent to users

      $tousers = $this->db->query("SELECT `$column`-(`$column` % 86400) AS `day`, `to`, COUNT(*) AS `count`, SUM(`size`) AS `size` FROM " . VIEW_MESSAGES . " WHERE `$column` >= ? AND `$column` < ? GROUP BY FROM_UNIXTIME(`day`, '%Y.%m.%d.'), `to`", array($start, $stop));

      foreach($tousers->rows as $row) {
         $counter[$row['day']][$row['to']]['recd'] = $row['count'];
         $counter[$row['day']][$row['to']]['sizerecd'] = $row['size'];
      }


      // emails sent from users

      $fromusers = $this->db->query("SELECT `$column`-(`$column` % 86400) AS `day`, `from`, COUNT(*) AS `count`, SUM(`size`) AS `size` FROM " . VIEW_MESSAGES . " WHERE `$column` >= ? AND `$column` < ? GROUP BY FROM_UNIXTIME(`day`, '%Y.%m.%d.'), `from`", array($start, $stop));

      foreach($fromusers->rows as $row) {
         $counter[$row['day']][$row['from']]['sent'] = $row['count'];
         $counter[$row['day']][$row['from']]['sizesent'] = $row['size'];
      }


      $accepteddomains = array_flip($this->model_domain_domain->get_mapped_domains());

      foreach($counter as $date => $users) {

         // execute queries to update the users and domains counter table
         $deletestats = $this->db->query("DELETE FROM " . TABLE_STAT_COUNTER . " WHERE `date` = ?", array($date));
         $data['deletedstats'] += $this->db->countAffected();

         foreach($users as $username => $userdata) {

            $sent = isset($userdata['sent']) ? $userdata['sent'] : 0;
            $recd = isset($userdata['recd']) ? $userdata['recd'] : 0;
            $sizesent = isset($userdata['sizesent']) ? $userdata['sizesent'] : 0;
            $sizerecd = isset($userdata['sizerecd']) ? $userdata['sizerecd'] : 0;

            $parts = explode('@', $username);

            if(isset($parts[1]) && isset($accepteddomains[ $parts[1] ])) {
               $addusers = $this->db->query("INSERT INTO " . TABLE_STAT_COUNTER . " (`date`,`email`,`domain`,`sent`,`recd`,`sentsize`,`recdsize`) VALUES(?,?,?,?,?,?,?)", array($date, $username, $parts[1], $sent, $recd, $sizesent, $sizerecd));
               $data['addedstats'] += $this->db->countAffected();
            }
         }
      }

      if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, sprintf("processed %s to %s: %d records deleted, %d records added", date(DATE_TEMPLATE, $data['starttimestamp']), date(DATE_TEMPLATE, $data['stoptimestamp']), $data['deletedstats'], $data['addedstats'])); }

      return $data;
   }


   public function get_accounting($item='email', $search='', $page=0, $pagelen=0, $sort='item', $order=0) {

      // item can be either email or domain, maybe folder in the future??

      $_order = 0;
      $_order = "";
      $limit = "";

      $account_for_emails = $this->__getEmails();
      $account_for_domains = $this->__getDomains();

      $search = preg_replace("/\s{1,}/", "", $search);

      if($item == 'email') {
         $account_for_emails = $this->__getEmails();
         $account_for_domains = $this->__getDomains();
         $query = "SELECT `email` AS `item`,MIN(`date`) as `oldest`,MAX(`date`) as `newest`,sum(`sent`) as `sent`,sum(`recd`) as `recd`,SUM(`sentsize`) as `sentsize`,AVG(`sentsize`) as `sentavg`,SUM(`recdsize`) as `recdsize`,AVG(`recdsize`) as `recdavg` FROM " . TABLE_STAT_COUNTER;
         $where = "WHERE ( `email` IN ('".implode("','",$account_for_emails)."') OR `domain` IN ('".implode("','",$account_for_domains)."') )";
         if($search) {
            $where .= " AND ( `email` like '%".$search."%' OR `domain` like '%".$search."%' )";
         }
         $group = "GROUP BY `email`";
      } elseif ($item == 'domain') {
         $account_for_domains = $this->__getDomains();
         $query = "SELECT `domain` AS `item`,MIN(`date`) as `oldest`,MAX(`date`) as `newest`,sum(`sent`) as `sent`,sum(`recd`) as `recd`,SUM(`sentsize`) as `sentsize`,AVG(`sentsize`) as `sentavg`,SUM(`recdsize`) as `recdsize`,AVG(`recdsize`) as `recdavg` FROM " . TABLE_STAT_COUNTER;
         $where = "WHERE ( `domain` IN ('".implode("','",$account_for_domains)."') )";
         if($search) {
            $where .= " AND `domain` like '%".$search."%'";
         }
         $group = "GROUP BY `domain`";
      } else {
         return false;
      }

      if($order == 0) { $order = "ASC"; }
      else { $order = "DESC"; }

      $_order = "ORDER BY `$sort` $order";

      $from = (int)$page * (int)$pagelen;

      if($pagelen > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$pagelen; }

      $query = $this->db->query($query.' '.$where.' '.$group.' '.$_order.' '.$limit.';');

      if($query->num_rows >= 1) {
         return $query->rows;
      } else {
         // no results found
         return false;
      }
   }


   public function count_accounting($item='email', $search='') {
      $account_for_emails = $this->__getEmails();
      $account_for_domains = $this->__getDomains();

      $search = preg_replace("/\s{1,}/", "", $search);

      if($search) {
         $search_cond .= " AND ( `email` LIKE '%".$search."%' OR `domain` LIKE '%".$search."%' )";
      }

      $query = "SELECT `email` AS `item`, MIN(`date`) AS `oldest`, MAX(`date`) AS `newest`, SUM(`sent`) AS `sent`, SUM(`recd`) AS `recd`, SUM(`sentsize`) AS `sentsize`, SUM(`recdsize`) AS `recdsize` FROM " . TABLE_STAT_COUNTER;

      if($item == 'email') {
         $where = "WHERE `email` IN ('".implode("','",$account_for_emails)."') OR `domain` IN ('".implode("','",$account_for_domains)."')";
         if($search) {
            $where .= " AND ( `email` LIKE '%".$search."%' OR `domain` LIKE '%".$search."%' )";
         }
         $group = "GROUP BY `email`";
      } elseif ($item == 'domain') {
        $where = "WHERE `domain` IN ('".implode("','",$account_for_domains)."')";
        if($search) {
           $where .= " AND `domain` LIKE '%".$search."%'";
        }
        $group = "GROUP BY `domain`";
      } else {
         return false;
      }

      $query = $this->db->query($query.' '.$where.' '.$group);

      return $query->num_rows;
   }


   private function __getEmails() {
      $emails = array();
      $session = Registry::get('session');

      array_push($emails, $session->get("email"));
      $__emails = $session->get("emails");

      foreach ($__emails as $e) {
         array_push($emails, $e);
      }

      return $emails;
   }


   private function __getDomains() {
      $domains = array();
      $session = Registry::get('session');

      if(Registry::get('admin_user') >= 1) {
         $domains = $this->model_domain_domain->get_mapped_domains();
      }
      elseif(Registry::get('auditor_user') == 1) {
         array_push($domains, $session->get("domain"));
         $auditdomains = $session->get("auditdomains");

         foreach ($auditdomains as $d) {
            array_push($domains, $d);
         }
      }

      return $domains;
   }


   private function __decodeDate($input) {
      $timestamp = 0;

      if(!is_numeric($input)) {
         // if we got anything, but a timestamp, eg. words (yesterday, today)
         // then convert to unix timestamp representing the start of the day
         $input = strtotime($input);
         $timestamp = $input - ($input % 86400);
      }
      else {
         // round the timestamp to the start of the day
         $timestamp = $input - ($input % 86400);
      }

      return $timestamp;
   }


}
