<div style="display:flex;justify-content:center;">
    <div>
        <img src="/public/images//cover-rectangle-blue.jpg" style="width: 200px;">
        <img src="/public/images//cover-rectangle-margin.jpg" style="width: 200px;">
        <img src="/public/images//cover-rectangle.jpg" style="width: 200px;">
    </div>
</div>

# Focused

Focused is a tool designed to help you stay focused longer during work by blocking distracting websites and resources. It modifies your `/etc/hosts` file to redirect specified sites to `127.0.0.1`. Additionally, it features a quadratic equation solver that makes you think² twice before unblocking distractors.


## Features

- 🚀 **Blocks Distracting Websites**: Easily block access to distracting sites.
- 📚 **Educational Challenge**: Solve a quadratic equation to unblock the sites.
- 🌐 **Cross-Platform Support**: Works on macOS, Linux, and Windows (with appropriate modifications).
- 💪 **Extendable**: Easily add new distractors and contribute to the program.

## Installation

1. **Clone the repository:**

    ```bash
    git clone https://github.com/yourusername/focused.git
    cd focused
    ```

2. **Compile the program:**

    ```bash
    g++ -std=c++17 -o focused focused.cpp
    ```

## Usage

1. **Run the program:**

    ```bash
    sudo ./focused
    ```

    > Note: `sudo` is required to modify the `/etc/hosts` file.

2. **Follow the prompts:**

    The program will ask if you want to enable or disable blocking social networks.

3. **Adding New Distractors:**

    Add new distractors to the `data/distractors.txt` file. Comments and empty lines are ignored.

    ```text
    # Social Networks
    facebook.com
    twitter.com
    instagram.com
    linkedin.com
    reddit.com

    # Example IP Address
    123.45.67.89
    ```

## Contribution

We welcome everyone who wants to add new distractors and commit to our program, making life more free from distractions. Feel free to fork the repository, make your changes, and submit a pull request.

## Future Releases

- **Qt GUI**: We plan to develop a graphical user interface using the Qt framework.
- **Mobile Support**: Adding support for iOS and Android platforms.
- **Cross-Device Usage**: Sync blocking settings across multiple devices.

## Benefits

- 🎯 **Stay Focused**: Enhance your productivity by blocking distracting websites.
- 🔒 **Privacy First**: Your data stays local, no external servers involved.
- 🚧 **Customizable**: Tailor the list of distractors to your personal needs.

---

Let's build a more focused and productive world together! 🚀