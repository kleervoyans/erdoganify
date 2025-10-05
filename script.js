const CANVAS_SIZE = 480;
const CELL_SIZE = 4;
const HALF_CELL = CELL_SIZE / 2;

const canvas = document.getElementById("erdoganCanvas");
const ctx = canvas.getContext("2d");
const statusMessage = document.getElementById("statusMessage");
const fileInput = document.getElementById("imageInput");

const sourceCanvas = document.createElement("canvas");
sourceCanvas.width = CANVAS_SIZE;
sourceCanvas.height = CANVAS_SIZE;
const sourceCtx = sourceCanvas.getContext("2d");

const targetCanvas = document.createElement("canvas");
targetCanvas.width = CANVAS_SIZE;
targetCanvas.height = CANVAS_SIZE;
const targetCtx = targetCanvas.getContext("2d");

let targetCells = [];
let particles = [];
let animationFrame = null;
let lastTimestamp = 0;
let targetReady = false;

class Particle {
  constructor(start, target, delay) {
    this.startX = start.x;
    this.startY = start.y;
    this.startColor = start.color;

    this.targetX = target.x;
    this.targetY = target.y;
    this.targetColor = target.color;

    this.delay = delay;
    this.progress = 0;
    this.speed = 0.65 + Math.random() * 0.45;
    this.finished = false;
  }

  update(delta) {
    if (this.finished) return;

    if (this.delay > 0) {
      this.delay = Math.max(0, this.delay - delta);
      return;
    }

    this.progress = Math.min(1, this.progress + delta * this.speed);
    if (this.progress >= 1) {
      this.finished = true;
    }
  }

  draw(context) {
    const effectiveProgress = this.delay > 0 ? 0 : easeOutCubic(this.progress);
    const x = lerp(this.startX, this.targetX, effectiveProgress);
    const y = lerp(this.startY, this.targetY, effectiveProgress);

    const r = Math.round(
      lerp(this.startColor[0], this.targetColor[0], effectiveProgress)
    );
    const g = Math.round(
      lerp(this.startColor[1], this.targetColor[1], effectiveProgress)
    );
    const b = Math.round(
      lerp(this.startColor[2], this.targetColor[2], effectiveProgress)
    );
    const a = lerp(this.startColor[3], this.targetColor[3], effectiveProgress);

    context.fillStyle = `rgba(${r}, ${g}, ${b}, ${a})`;
    context.fillRect(x - HALF_CELL, y - HALF_CELL, CELL_SIZE, CELL_SIZE);
  }
}

function lerp(start, end, t) {
  return start + (end - start) * t;
}

function easeOutCubic(t) {
  return 1 - Math.pow(1 - t, 3);
}

function shuffle(array) {
  const copy = [...array];
  for (let i = copy.length - 1; i > 0; i -= 1) {
    const j = Math.floor(Math.random() * (i + 1));
    [copy[i], copy[j]] = [copy[j], copy[i]];
  }
  return copy;
}

function drawImageCover(context, image) {
  const canvasWidth = context.canvas.width;
  const canvasHeight = context.canvas.height;
  const canvasRatio = canvasWidth / canvasHeight;
  const imageRatio = image.width / image.height;

  let drawWidth;
  let drawHeight;
  let offsetX;
  let offsetY;

  if (imageRatio > canvasRatio) {
    drawHeight = canvasHeight;
    drawWidth = drawHeight * imageRatio;
    offsetX = (canvasWidth - drawWidth) / 2;
    offsetY = 0;
  } else {
    drawWidth = canvasWidth;
    drawHeight = drawWidth / imageRatio;
    offsetX = 0;
    offsetY = (canvasHeight - drawHeight) / 2;
  }

  context.clearRect(0, 0, canvasWidth, canvasHeight);
  context.drawImage(image, offsetX, offsetY, drawWidth, drawHeight);
}

function imageDataToCells(imageData) {
  const cells = [];
  const { width, height, data } = imageData;

  for (let y = HALF_CELL; y < height; y += CELL_SIZE) {
    for (let x = HALF_CELL; x < width; x += CELL_SIZE) {
      const clampedX = Math.min(Math.floor(x), width - 1);
      const clampedY = Math.min(Math.floor(y), height - 1);
      const index = (clampedY * width + clampedX) * 4;
      cells.push({
        x,
        y,
        color: [
          data[index],
          data[index + 1],
          data[index + 2],
          data[index + 3] / 255,
        ],
      });
    }
  }

  return cells;
}

function prepareParticles(sourceCells) {
  const shuffledSource = shuffle(sourceCells);

  particles = targetCells.map((targetCell, index) => {
    const startCell = shuffledSource[index % shuffledSource.length];
    const delay = Math.random() * 0.6 + (index / targetCells.length) * 0.4;
    return new Particle(startCell, targetCell, delay);
  });
}

function animate(timestamp) {
  if (!lastTimestamp) {
    lastTimestamp = timestamp;
  }
  const delta = (timestamp - lastTimestamp) / 1000;
  lastTimestamp = timestamp;

  ctx.clearRect(0, 0, CANVAS_SIZE, CANVAS_SIZE);

  let allFinished = true;
  for (const particle of particles) {
    particle.update(delta);
    particle.draw(ctx);
    if (!particle.finished) {
      allFinished = false;
    }
  }

  if (!allFinished) {
    animationFrame = requestAnimationFrame(animate);
  } else {
    statusMessage.textContent = "Transformation complete.";
    animationFrame = null;
  }
}

function startTransformation(image) {
  drawImageCover(sourceCtx, image);
  const sourceData = sourceCtx.getImageData(0, 0, CANVAS_SIZE, CANVAS_SIZE);
  const sourceCells = imageDataToCells(sourceData);

  prepareParticles(sourceCells);

  if (animationFrame) {
    cancelAnimationFrame(animationFrame);
  }

  lastTimestamp = 0;
  statusMessage.textContent = "Reorganising pixels…";
  animationFrame = requestAnimationFrame(animate);
}

fileInput.addEventListener("change", (event) => {
  const file = event.target.files?.[0];
  if (!file) {
    return;
  }

  if (!targetReady) {
    statusMessage.textContent = "Still loading the target Erdogan image…";
    return;
  }

  const url = URL.createObjectURL(file);
  const img = new Image();

  img.onload = () => {
    URL.revokeObjectURL(url);
    startTransformation(img);
  };

  img.onerror = () => {
    URL.revokeObjectURL(url);
    statusMessage.textContent = "Unable to read that file. Try another image.";
  };

  img.src = url;
  statusMessage.textContent = "Preparing your image…";
});

const erdoganImage = new Image();

erdoganImage.onload = () => {
  drawImageCover(targetCtx, erdoganImage);
  const targetData = targetCtx.getImageData(0, 0, CANVAS_SIZE, CANVAS_SIZE);
  targetCells = imageDataToCells(targetData);
  targetReady = true;
  statusMessage.textContent = "Choose an image to begin.";
};

erdoganImage.onerror = () => {
  statusMessage.textContent = "Failed to load the Erdogan reference image.";
};

erdoganImage.src = "assets/erdogan.jpg";
