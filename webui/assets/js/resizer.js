const resizer = document.querySelector('.resizer');
const upperPane = document.querySelector('.pane-upper');
const lowerPane = document.querySelector('.pane-lower');
const containerPanes = document.querySelector('.container-panes');

let isResizing = false;

resizer.addEventListener('mousedown', (e) => {
  isResizing = true;
  document.body.classList.add('no-select');
});

document.addEventListener('mousemove', (e) => {
  if (!isResizing) return;
  let offsetY = e.clientY - containerPanes.getBoundingClientRect().top;
  upperPane.style.height = `${offsetY}px`;
  lowerPane.style.height = `calc(100% - ${offsetY + resizer.offsetHeight}px)`;
});

document.addEventListener('mouseup', () => {
  isResizing = false;
  document.body.classList.remove('no-select');
});
