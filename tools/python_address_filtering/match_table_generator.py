import tkinter as tk
from tkinter import messagebox

class AddressFilterApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Address Filtering - Match Table Calculator")

        # Table dimensions and bits
        self.table_rows = 6
        self.table_cols = 6
        self.bits = [[None for _ in range(self.table_cols)] for _ in range(self.table_rows)]

        # FIELD 1 label
        self.table_label_frame = tk.Frame(root)
        self.table_label_frame.pack(pady=5)
        tk.Label(self.table_label_frame, text="FIELD 1").grid(row=0, column=0, sticky="w")

        # Frame for the table
        self.table_frame = tk.Frame(root)
        tk.Label(self.table_frame, text="FIELD \n0", wraplength=1).grid(rowspan=10, column=0, sticky="w")
        self.table_frame.pack(pady=10)

        self.create_table()

        # Frame for buttons and entry
        self.control_frame = tk.Frame(root)
        self.control_frame.pack(pady=10)

        self.create_controls()

    def create_table(self):
        self.entries = []
        headers = ["", "No match", "Address 0", "Address 1", "Address 2", "Address 3"]
        row_labels = ["No match", "Address 0", "Address 1", "Address 2", "Address 3"]

        for i in range(self.table_rows):
            row_entries = []
            for j in range(self.table_cols):
                if i == 0:  # Header row
                    label = tk.Label(self.table_frame, text=headers[j], width=10, relief="ridge")
                elif j == 0:  # Row labels
                    label = tk.Label(self.table_frame, text=row_labels[i-1], width=10, relief="ridge")
                else:
                    entry = tk.Entry(self.table_frame, width=5, justify="center")
                    entry.insert(0, "0")
                    entry.grid(row=i, column=j+1)
                    row_entries.append(entry)
                    continue
                label.grid(row=i, column=j+1)
            if i != 0:
                self.entries.append(row_entries)

    def create_controls(self):
        tk.Label(self.control_frame, text="Generated Variable (Binary):").grid(row=0, column=0, sticky="w")
        self.binary_result = tk.Entry(self.control_frame, width=40, state="readonly")
        self.binary_result.grid(row=0, column=1)
        tk.Button(self.control_frame, text="Copy", command=self.copy_binary).grid(row=0, column=2)

        tk.Label(self.control_frame, text="Generated Variable (Hex):").grid(row=1, column=0, sticky="w")
        self.hex_result = tk.Entry(self.control_frame, width=40, state="readonly")
        self.hex_result.grid(row=1, column=1)
        tk.Button(self.control_frame, text="Copy", command=self.copy_hex).grid(row=1, column=2)

        tk.Button(self.control_frame, text="Convert Table to Variable", command=self.generate_variable).grid(row=2, column=0, pady=5)
        tk.Button(self.control_frame, text="Convert Variable to Table", command=self.convert_to_table).grid(row=2, column=1, pady=5)

        self.variable_entry = tk.Entry(self.control_frame, width=40)
        self.variable_entry.grid(row=3, column=0, columnspan=2, pady=5)
        tk.Label(self.control_frame, text="Enter Variable to Convert (Hex or Binary)").grid(row=3, column=2, sticky="w")

    def generate_variable(self):
        binary = ["0"] * 32
        bit_idx = 1

        for i in range(0, self.table_rows-1):
            for j in range(0, self.table_cols-1):
                value = self.entries[i][j].get()
                if value == "1":
                    binary[-bit_idx] = "1"
                elif value == "0":
                    binary[-bit_idx] = "0"
                else:
                    messagebox.showerror("Invalid Input", "Only binary values are allowed in the table!")
                    return
                bit_idx += 1

        binary_str = "".join(binary[-32:])  # Ensure only 32 bits are used
        hex_str = hex(int(binary_str, 2))

        self.binary_result.config(state="normal")
        self.binary_result.delete(0, tk.END)
        self.binary_result.insert(0, binary_str)
        self.binary_result.config(state="readonly")

        self.hex_result.config(state="normal")
        self.hex_result.delete(0, tk.END)
        self.hex_result.insert(0, hex_str)
        self.hex_result.config(state="readonly")

    def convert_to_table(self):
        variable = self.variable_entry.get().strip()
        try:
            if variable.startswith("0x"):
                binary = bin(int(variable, 16))[2:].zfill(32)
            else:
                binary = bin(int(variable, 2))[2:].zfill(32)
        except ValueError:
            messagebox.showerror("Invalid Input", "Enter a valid binary or hexadecimal variable.")
            return

        bit_idx = 1
        for i in range(0, self.table_rows-1):
            for j in range(0, self.table_cols-1):
                if bit_idx <= len(binary):
                    self.entries[i][j].delete(0, tk.END)
                    self.entries[i][j].insert(0, binary[-bit_idx])
                else:
                    self.entries[i][j].delete(0, tk.END)
                bit_idx += 1

    def copy_binary(self):
        self.root.clipboard_clear()
        self.root.clipboard_append(self.binary_result.get())
        self.root.update()

    def copy_hex(self):
        self.root.clipboard_clear()
        self.root.clipboard_append(self.hex_result.get())
        self.root.update()

if __name__ == "__main__":
    root = tk.Tk()
    app = AddressFilterApp(root)
    root.mainloop()
