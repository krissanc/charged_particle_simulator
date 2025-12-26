# React Download Page Example

This is an example React component you can use on your website to provide downloads of the Charged Particle Simulator executable.

## Simple Download Component

```jsx
import React, { useState } from 'react';

function SimulatorDownload() {
  const [downloadStarted, setDownloadStarted] = useState(false);

  // URL to your hosted executable or ZIP file
  const downloadUrl = '/downloads/ChargedParticleSim_v1.0.zip';
  const fileSize = '~5 MB'; // Update with actual size

  const handleDownload = () => {
    setDownloadStarted(true);
    // Create a temporary anchor element to trigger download
    const link = document.createElement('a');
    link.href = downloadUrl;
    link.download = 'ChargedParticleSim_v1.0.zip';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    
    // Reset after a delay
    setTimeout(() => setDownloadStarted(false), 2000);
  };

  return (
    <div className="max-w-4xl mx-auto p-8">
      <div className="bg-white rounded-lg shadow-lg p-8">
        <h1 className="text-4xl font-bold mb-4 text-gray-800">
          Charged Particle Simulator
        </h1>
        
        <p className="text-xl text-gray-600 mb-8">
          A real-time, physically-accurate visualization of charged particles 
          and their electromagnetic fields in 3D space.
        </p>

        <div className="bg-blue-50 border border-blue-200 rounded-lg p-6 mb-8">
          <h2 className="text-2xl font-semibold mb-4 text-blue-900">
            Download Now
          </h2>
          
          <div className="mb-4">
            <p className="text-gray-700 mb-2">
              <strong>Version:</strong> 1.0
            </p>
            <p className="text-gray-700 mb-2">
              <strong>File Size:</strong> {fileSize}
            </p>
            <p className="text-gray-700">
              <strong>Platform:</strong> Windows 10/11 (64-bit)
            </p>
          </div>

          <button
            onClick={handleDownload}
            disabled={downloadStarted}
            className="bg-blue-600 hover:bg-blue-700 disabled:bg-blue-400 
                     text-white font-bold py-3 px-8 rounded-lg 
                     transition-colors duration-200 
                     shadow-lg hover:shadow-xl
                     disabled:cursor-not-allowed"
          >
            {downloadStarted ? 'Downloading...' : 'Download for Windows'}
          </button>
        </div>

        <div className="grid md:grid-cols-2 gap-6 mb-8">
          <div>
            <h3 className="text-xl font-semibold mb-3 text-gray-800">
              System Requirements
            </h3>
            <ul className="list-disc list-inside space-y-2 text-gray-700">
              <li>Windows 10 or later (64-bit)</li>
              <li>OpenGL 4.3+ compatible graphics card</li>
              <li>Visual C++ Redistributable 2015-2022</li>
              <li>4 GB RAM minimum</li>
              <li>100 MB free disk space</li>
            </ul>
          </div>

          <div>
            <h3 className="text-xl font-semibold mb-3 text-gray-800">
              Features
            </h3>
            <ul className="list-disc list-inside space-y-2 text-gray-700">
              <li>Physically accurate Coulomb's law simulation</li>
              <li>Interactive particle manipulation</li>
              <li>Real-time field line visualization</li>
              <li>3D camera controls</li>
              <li>Multiple particle support</li>
            </ul>
          </div>
        </div>

        <div className="border-t pt-6">
          <h3 className="text-xl font-semibold mb-3 text-gray-800">
            Installation Instructions
          </h3>
          <ol className="list-decimal list-inside space-y-2 text-gray-700">
            <li>Download the ZIP file above</li>
            <li>Extract all files to a folder on your computer</li>
            <li>Run <code className="bg-gray-100 px-2 py-1 rounded">ChargedParticleSim.exe</code></li>
            <li>If you see a security warning, click "More info" and "Run anyway"</li>
          </ol>
        </div>

        <div className="border-t pt-6 mt-6">
          <h3 className="text-xl font-semibold mb-3 text-gray-800">
            Controls
          </h3>
          <ul className="space-y-2 text-gray-700">
            <li><strong>Left Mouse Drag:</strong> Orbit camera around scene</li>
            <li><strong>Left Click + Drag on Particle:</strong> Drag particle</li>
            <li><strong>Scroll Wheel:</strong> Zoom in/out</li>
            <li><strong>R Key:</strong> Reset camera</li>
            <li><strong>SPACE:</strong> Pause/resume simulation</li>
            <li><strong>ESC:</strong> Exit application</li>
          </ul>
        </div>

        <div className="border-t pt-6 mt-6">
          <h3 className="text-xl font-semibold mb-3 text-gray-800">
            Troubleshooting
          </h3>
          <div className="space-y-3 text-gray-700">
            <div>
              <strong>Application won't start:</strong>
              <ul className="list-disc list-inside ml-4 mt-1">
                <li>Make sure <code className="bg-gray-100 px-1 rounded">glfw3.dll</code> is in the same folder</li>
                <li>Install Visual C++ Redistributable from Microsoft</li>
                <li>Update your graphics drivers</li>
              </ul>
            </div>
            <div>
              <strong>Graphics errors:</strong>
              <ul className="list-disc list-inside ml-4 mt-1">
                <li>Ensure your graphics card supports OpenGL 4.3+</li>
                <li>Update graphics drivers to latest version</li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default SimulatorDownload;
```

## Usage in Your React App

1. **Place the component** in your React app (e.g., `src/components/SimulatorDownload.jsx`)

2. **Add the route** in your router:
```jsx
import SimulatorDownload from './components/SimulatorDownload';

// In your router
<Route path="/download" element={<SimulatorDownload />} />
```

3. **Upload the executable/ZIP** to your public folder:
   - Create `public/downloads/` folder
   - Place `ChargedParticleSim_v1.0.zip` there
   - Update the `downloadUrl` in the component

4. **Style with Tailwind** (as per your preferences):
   - The example uses Tailwind classes
   - Adjust colors and spacing to match your theme

## Alternative: Direct Link

If you prefer a simpler approach, just use a direct download link:

```jsx
<a 
  href="/downloads/ChargedParticleSim_v1.0.zip" 
  download
  className="bg-blue-600 text-white px-6 py-3 rounded-lg hover:bg-blue-700"
>
  Download Charged Particle Simulator
</a>
```

## File Hosting Options

### Option 1: Same Server
- Place file in `public/downloads/` folder
- Access via `/downloads/filename.zip`

### Option 2: CDN
- Upload to AWS S3, Cloudflare, or similar
- Use CDN URL in download link
- Better for large files

### Option 3: GitHub Releases
- Create a GitHub release
- Link directly to release asset
- Free hosting with versioning

## Analytics Tracking

To track downloads, you can add analytics:

```jsx
const handleDownload = () => {
  // Track download event
  if (window.gtag) {
    gtag('event', 'download', {
      'event_category': 'Simulator',
      'event_label': 'ChargedParticleSim_v1.0'
    });
  }
  
  // Your download logic here
  handleDownload();
};
```

