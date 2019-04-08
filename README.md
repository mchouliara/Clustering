Υλοποίηση αλγορίθμων για την συσταδοποίηση δυανισμάτων, χρησιμοποιώντας διάφορους συνδιασμούς αλγορίθμων για initialization, assignment και update

Οι επιλογές αλγορίθμων είναι:

Initialization
1. Random selection of k points (simplest)
2. K-means++

Assignment
1. Lloyd’s assignment
2. Assignment by Range search with LSH
3. Assignment by Range search with Hypercube

Update
1. K-means
2. Partitioning Around Medoids (PAM) improved like Lloyd’s

Το πρόγραμμα δέχεται το dataset, τρέχει 12 συνδιασμούς αλγόριθμων για το clustering των δεδομένων και παραθέτει τα αποτελέσματα τους σε ενα output αρχείο, το όνομα του οποίου έχει καθορίστει απο τον χρήστη. Το αρχείο cluster.config που δέχεται το πρόγραμμα μπορεί να περιέχει όσα δεδομένα ζητούνται απο την εκφώνηση, συν μιας έξτρα παραμέτρου max_loops.
Λόγω της χρονοβόρας φύσης του προγράμματος, υπάρχει συνάρτηση που επιτρέπει την εκτέλεση ενός συνδιασμού αλγορίθμων ξεχωριστά.

Μεταγλώττιση: make all / make clean

Χρήση: ./cluster  –i  (input  file)  –c  (configuration  file)  -o  (output  file)  -d  (metric)  (-complete optional)

Αρχεία: 

class.functions.h  
class_functions.cpp:	εσωτερικές συναρτήσεις των κλάσεων που δηλώνονται στο header  
math_functions.h  
math_functions.cpp:	συναρτήσεις μαθηματικών υπολογισμών  
filedata.h  
filedata.cpp:		συναρτήσεις parsing αρχείων και command line  
lsh.h  
lsh.cpp:		 συναρτήσεις lsh, hypercube για range search  
cluster.h  
cluster.cpp:		συναρτήσεις clustering  
main.cpp  
Makefile  

Επιπρόσθετα:

cluster_config: Αρχείο με ενδεικτικό configuration του εκτελέσιμου  
/results/output: Ενδεικτικά αποτελέσματα της εκτέλεσης ./cluster -i twitter_dataset_small.csv -c cluster_config -o output_complete -d euclidean με το cluster_config  
/results/output_complete: Ενδεικτικά αποτελέσματα ίδιας εκτέλεσης, με -complete περασμένο ως παράμετρο απο την γραμμή εντολών  
/results/compare.pdf: Παράθεση και συγκρίσεις των αποτελεσμάτων των διαφορετικών αλγόριθμων




