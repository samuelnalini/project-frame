
# Front-End Web Framework

# Goal

Honestly I haven't really decided what this will be yet.

# Components

A component is a section of reusable code that can be inserted into raw HTML.

Files to be processed should have a *.process* extension.

## Creating a Component

Components are stored in a folder called **components**

The processor will read the files in the **components** dir and store them until they are needed

The syntax goes as follow:

```
@MyComponent {
    // html, css, or javascript
}
```

Whatever is placed inside of the component will be copied where a reference to it is made.

## Using Components

Once a component is created, it can be referenced in *.process* files.

```
${MyComponent}
```


This project is on the backburner a bit as I'm working on learning new concepts in C. I did not abandon it.
