from PIL import Image

def resize_to_240(input_path, output_path):
    # Open the image
    with Image.open(input_path) as img:
        # Convert to RGB (required for JPG, especially if source is PNG/RGBA)
        img = img.convert("RGB")
        
        # Resize to exactly 240x240
        # LANCZOS is used for high-quality downsampling
        resized_img = img.resize((240, 240), Image.Resampling.LANCZOS)
        
        # Save as JPEG
        resized_img.save(output_path, "JPEG")
        print(f"Image saved to {output_path}")

# Example usage
resize_to_240("C:/ZionProjects/Arduino_GFX_2026/Arduino_GFX_sfcgta/resize_image/10.png", "C:/ZionProjects/Arduino_GFX_2026/Arduino_GFX_sfcgta/10.jpg")
