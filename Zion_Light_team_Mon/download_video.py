import yt_dlp
import time
import os
import questionary
import cv2
# !pip install yt_dlp


def download_video(entry, download_dir="./downloads"):
    """Downloads a specific YouTube entry as 480p MP4."""
    if not os.path.exists(download_dir):
        os.makedirs(download_dir)

    ydl_opts = {
        'format': 'bestvideo[height<=480][ext=mp4]+bestaudio[ext=m4a]/best[height<=480][ext=mp4]',
        'outtmpl': f'{download_dir}/%(title)s.%(ext)s',
        'postprocessors': [{'key': 'FFmpegVideoConvertor', 'preferedformat': 'mp4'}],
        'quiet': True,
        'no_warnings': True
    }

    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info = ydl.extract_info(entry['url'], download=True)
        return ydl.prepare_filename(info)


def get_video_metadata(url):
    """Fetches video/playlist info. Returns (title, list_of_entries, is_playlist)."""
    ydl_opts = {'extract_flat': True, 'quiet': True}
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info = ydl.extract_info(url, download=False)
        if 'entries' in info:  # It's a playlist
            return info['title'], list(info['entries']), True
        else:  # It's a single video
            return info['title'], [info], False


def main():
    url = input("Enter YouTube Video or Playlist URL: ").strip()

    print("Gathering information...")
    title, entries, is_playlist = get_video_metadata(url)

    if is_playlist:
        choices = [
            questionary.Choice(title=f"{i+1}. {e.get('title')}", value=e)
            for i, e in enumerate(entries)
        ]
        selected_entries = questionary.checkbox(
            f"Playlist: {title}\nSelect videos for OCR extraction:",
            choices=choices
        ).ask()
    else:
        print(f"Single video detected: {title}")
        selected_entries = entries

    if not selected_entries:
        print("No videos selected.")
        return

    # Process each selected video
    for entry in selected_entries:
        print(f"\n--- Processing: {entry.get('title')} ---")

        # 1. Download
        local_path = download_video(entry)

        # 2. Extract Subtitles (using your sunday_extract logic)
        if os.path.exists(local_path):
            print("Downloaded the video to ", local_path)

            # texts, out_file = process_video_frames(
            #     local_path, frames_per_second=1)
            # if out_file:
            #     print(f"✅ Subtitles saved to: {out_file}")

            # Optional: Clean up video file after extraction to save space
            # os.remove(local_path)
        else:
            print(f"❌ Download failed for {entry.get('title')}")


if __name__ == "__main__":
    main()
