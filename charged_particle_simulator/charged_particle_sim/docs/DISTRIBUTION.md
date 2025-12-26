# Distribution Guide

This guide explains how to build and distribute the Charged Particle Simulator as a downloadable executable for your website.

## Building a Release Package

### Quick Start

1. Run the release build script:
```bash
build_release.bat
```

This will:
- Build the project in Release mode
- Copy the executable to a `release` folder
- Copy required DLLs (GLFW)
- Create a README file
- Package everything for distribution

### Manual Build Process

If you prefer to build manually:

1. **Configure CMake for Release:**
```bash
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

2. **Build in Release mode:**
```bash
cmake --build . --config Release
```

3. **Locate the executable:**
   - Usually in `build/Release/ChargedParticleSim.exe`

4. **Copy required files:**
   - `ChargedParticleSim.exe` - Main executable
   - `glfw3.dll` - GLFW library (required)
   - `shaders/` folder - If you have custom shaders
   - Any config files

5. **Create a ZIP file** with all these files

## Required DLLs

### GLFW DLL (glfw3.dll)

The application requires `glfw3.dll` to run. Common locations:

- **If using vcpkg:**
  - `C:\vcpkg\installed\x64-windows\bin\glfw3.dll`

- **If manually installed:**
  - Check your GLFW installation's `bin` or `lib` folder

- **If using pre-built binaries:**
  - Download from https://www.glfw.org/download.html
  - Extract and find `glfw3.dll` in the `lib-vc2022` or `lib-vc2019` folder

**Important:** The DLL must match the architecture (x64) of your executable.

### Visual C++ Redistributable

Users will need the Visual C++ Redistributable installed. Most Windows systems already have this, but you can provide a download link:

- **Microsoft Visual C++ Redistributable:** https://aka.ms/vs/17/release/vc_redist.x64.exe

## Creating a Distribution Package

### Option 1: Simple ZIP File

1. Create a folder: `ChargedParticleSim_v1.0`
2. Copy all required files into it
3. Create a ZIP file
4. Upload to your website

### Option 2: Installer (Advanced)

For a more professional distribution, consider using:

- **NSIS (Nullsoft Scriptable Install System)** - Free, open-source
- **Inno Setup** - Free, easy to use
- **WiX Toolset** - Microsoft's installer framework

These can create `.exe` installers that:
- Install to Program Files
- Create Start Menu shortcuts
- Add uninstaller
- Check for dependencies

## Website Integration

### Simple Download Page

Create a download page on your React website:

```jsx
// Example React component
function DownloadPage() {
  const handleDownload = () => {
    // Link to your hosted .exe or .zip file
    window.open('/downloads/ChargedParticleSim_v1.0.zip', '_blank');
  };

  return (
    <div>
      <h1>Download Charged Particle Simulator</h1>
      <p>Version 1.0</p>
      <button onClick={handleDownload}>Download Now</button>
      <div>
        <h2>System Requirements</h2>
        <ul>
          <li>Windows 10 or later</li>
          <li>OpenGL 4.3+ compatible graphics card</li>
          <li>Visual C++ Redistributable 2015-2022</li>
        </ul>
      </div>
    </div>
  );
}
```

### Hosting the File

1. **Upload to your web server:**
   - Place the ZIP file in a `public/downloads/` folder (for React)
   - Or upload to your server's download directory

2. **Update download link:**
   - Use absolute or relative path to the file
   - Example: `/downloads/ChargedParticleSim_v1.0.zip`

3. **Optional: Add version tracking:**
   - Track downloads
   - Provide release notes
   - Offer update notifications

## Security Considerations

### Code Signing (Recommended for Production)

To avoid Windows SmartScreen warnings:

1. **Get a code signing certificate:**
   - Purchase from a Certificate Authority (CA)
   - Or use self-signed for testing (not recommended for distribution)

2. **Sign your executable:**
```bash
signtool sign /f certificate.pfx /p password /t http://timestamp.digicert.com ChargedParticleSim.exe
```

3. **Benefits:**
   - No "Unknown Publisher" warnings
   - Better user trust
   - Required for some enterprise environments

### Antivirus False Positives

Sometimes antivirus software flags new executables. To minimize issues:

- Use code signing
- Submit to antivirus vendors for whitelisting
- Provide clear information about what the software does
- Consider using a well-known build environment

## Testing the Distribution

Before uploading:

1. **Test on a clean system:**
   - Use a VM or different computer
   - Ensure no development tools are installed
   - Test that all DLLs are included

2. **Verify:**
   - Application launches correctly
   - All features work
   - No missing file errors
   - Graphics render properly

3. **Check file size:**
   - Optimize if too large
   - Consider compression

## Version Management

### Naming Convention

Use semantic versioning:
- `ChargedParticleSim_v1.0.0.zip` - Major.Minor.Patch
- Include version in README
- Update version in code if needed

### Release Notes

Create a `CHANGELOG.txt` or include in README:
- Version number
- New features
- Bug fixes
- Known issues

## Automated Build (CI/CD)

For automated releases, you can:

1. **GitHub Actions** (if using GitHub):
   - Build on Windows runner
   - Create release package
   - Upload as release asset

2. **GitLab CI/CD:**
   - Similar workflow
   - Build and package automatically

3. **Local Script:**
   - Use `build_release.bat`
   - Automate with scheduled tasks

## Support

Provide users with:
- Download link
- System requirements
- Installation instructions
- Troubleshooting guide
- Contact/support information

