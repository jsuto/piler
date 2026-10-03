/* Drag handle between the result list and the preview pane.
   The panes live in a column flex container; once the user drags we take them
   out of the flex-grow distribution (flex: 0 0 auto) and give each an explicit
   height, otherwise the flex algorithm recalculates the inline height away and
   the drag has no visible effect. */
(function () {
  const resizer = document.querySelector('.resizer');
  const upperPane = document.querySelector('.pane-upper');
  const lowerPane = document.querySelector('.pane-lower');
  const containerPanes = document.querySelector('.container-panes');

  if (!resizer || !upperPane || !lowerPane || !containerPanes) return;

  const MIN_PANE = 120;
  let isResizing = false;

  resizer.addEventListener('mousedown', (e) => {
    isResizing = true;
    document.body.classList.add('no-select');
    e.preventDefault();
  });

  document.addEventListener('mousemove', (e) => {
    if (!isResizing) return;

    const containerRect = containerPanes.getBoundingClientRect();
    const resizerRect = resizer.getBoundingClientRect();

    // The handle has vertical margins too; count them so the explicit pane
    // heights sum to exactly the container height.
    const cs = getComputedStyle(resizer);
    const marginTop = parseFloat(cs.marginTop) || 0;
    const marginBottom = parseFloat(cs.marginBottom) || 0;
    const resizerTotal = resizerRect.height + marginTop + marginBottom;

    let upperHeight = e.clientY - containerRect.top - marginTop;
    const maxUpper = containerRect.height - resizerTotal - MIN_PANE;

    upperHeight = Math.max(MIN_PANE, Math.min(upperHeight, maxUpper));
    const lowerHeight = containerRect.height - upperHeight - resizerTotal;

    upperPane.style.flex = '0 0 auto';
    lowerPane.style.flex = '0 0 auto';
    upperPane.style.height = upperHeight + 'px';
    lowerPane.style.height = lowerHeight + 'px';
  });

  document.addEventListener('mouseup', () => {
    isResizing = false;
    document.body.classList.remove('no-select');
  });
})();
