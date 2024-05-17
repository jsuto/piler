<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="get" name="search1" action="/domain.php">
        <div class="row justify-content-end">
          <div class="col-8">
            <input type="text" name="search" class="form-control" value="<?php print $search; ?>">
          </div>
          <div class="col-2">
            <button type="submit" class="btn btn-primary"><?php print $text_search; ?></button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>


<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4><?php print $text_add_new_domain; ?></h4>

      <form method="post" name="add1" action="index.php?route=domain/domain">
      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="domain" class="col-form-label"><?php print $text_domain; ?></label>
        </div>
        <div class="col-2">
          <textarea name="domain" class="form-control" aria-describedby="help1"></textarea>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text">Domain name</span>
        </div>
      </div>
      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="mapped" class="col-form-label"><?php print $text_mapped_domain; ?></label>
        </div>
        <div class="col-2">
          <input type="text" name="mapped" class="form-control" aria-describedby="help2">
        </div>
        <div class="col-auto">
          <span id="help2" class="form-text">Mapped domain name</span>
        </div>
      </div>

<?php if(ENABLE_SAAS == 1) { ?>
      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="ldap" class="col-form-label"><?php print $text_ldap; ?></label>
        </div>
        <div class="col-2">
          <select name="ldap_id" id="ldap_id" class="form-control">
            <option value="0"></option>
          <?php foreach ($ldap as $l) { ?>
            <option value="<?php print $l['id']; ?>"><?php print $l['description']; ?></option>
          <?php } ?>
          </select>
        </div>
        <div class="col-auto">
          <span id="help2" class="form-text">Mapped domain name</span>
        </div>
      </div>
<?php } ?>
      <div class="row g-3 align-items-center">
        <div class="col-2">
        </div>
        <div class="col-2">
          <button type="submit" class="btn btn-primary"><?php print $text_add; ?></button>
          <button type="reset" class="btn btn-secondary"><?php print $text_cancel; ?></button>
        </div>
      </div>
      </form>
    </div>
  </div>
</div>


<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4 class="mt-5"><?php print $text_existing_domains; ?></h4>

      <table class="table table-striped">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_domain; ?></th>
            <th><?php print $text_mapped_domain; ?></th>
          <?php if(ENABLE_SAAS == 1) { ?>
            <th><?php print $text_ldap; ?></th>
          <?php } ?>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($domains as $domain) { ?>
          <tr>
            <td><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></td>
            <td><?php print $domain['mapped']; ?></td>
          <?php if(ENABLE_SAAS == 1) { ?>
            <td><?php if(isset($domain['ldap'])) {print $domain['ldap'];} else {print '&nbsp;';} ?></td>
          <?php } ?>
            <td><a href="index.php?route=domain/remove&amp;domain=<?php print urlencode($domain['domain']); ?>&amp;confirmed=1"><i class="bi bi-trash text-danger"></i></a></td>
          </tr>
        <?php } ?>
        </tbody>
      </table>
    </div>
  </div>
</div>
